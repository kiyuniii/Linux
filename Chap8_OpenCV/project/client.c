#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>                  /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <asm/types.h>               /* for videodev2.h */
#include <linux/videodev2.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#define FBDEV       "/dev/fb0"      /* Framebuffer device */
#define VIDEODEV    "/dev/video0"
#define WIDTH       800             /* Width of the captured video */
#define HEIGHT      600
#define TCP_PORT    5100            /* TCP port for communication */

static short *fbp = NULL;           /* Memory-mapped framebuffer pointer */
static struct fb_var_screeninfo vinfo; /* Structure to store framebuffer info */

/* Ensure the value does not exceed the range of unsigned char */
extern inline int clip(int value, int min, int max) {
    return(value > max ? max : value < min ? min : value);
}

/* Convert YUYV to RGB and render it on the framebuffer */
static void process_image(const void *p) {
    unsigned char* in = (unsigned char*)p;
    int width = WIDTH;
    int height = HEIGHT;
    int istride = WIDTH * 2;  /* Stride for YUYV format */
    int x, y, j;
    int y0, u, y1, v, r, g, b;
    unsigned short pixel;
    long location = 0;

    for(y = 0; y < height; ++y) {
        for(j = 0, x = 0; j < vinfo.xres * 2; j += 4, x += 2) {
            if(j >= width * 2) {
                 location++;
                 location++;
                 continue;
            }
            /* Extract YUYV components */
            y0 = in[j];
            u = in[j + 1] - 128;
            y1 = in[j + 2];
            v = in[j + 3] - 128;

            /* Convert YUYV to RGB */
            r = clip((298 * y0 + 409 * v + 128) >> 8, 0, 255);
            g = clip((298 * y0 - 100 * u - 208 * v + 128) >> 8, 0, 255);
            b = clip((298 * y0 + 516 * u + 128) >> 8, 0, 255);
            pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);  /* 16-bit color */
            fbp[location++] = pixel;

            r = clip((298 * y1 + 409 * v + 128) >> 8, 0, 255);
            g = clip((298 * y1 - 100 * u - 208 * v + 128) >> 8, 0, 255);
            b = clip((298 * y1 + 516 * u + 128) >> 8, 0, 255);
            pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);  /* 16-bit color */
            fbp[location++] = pixel;
        }
        in += istride;
    }
}

/* Function to ensure full data is read from the socket */
int read_full_data(int sockfd, unsigned char *buffer, int total_size) {
    int bytes_read = 0;
    int total_bytes_read = 0;

    while (total_bytes_read < total_size) {
        bytes_read = read(sockfd, buffer + total_bytes_read, total_size - total_bytes_read);
        
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                printf("Connection closed by the server.\n");
                break;
            } else {
                perror("Read error");
                break;
            }
        }

        total_bytes_read += bytes_read;
    }

    return total_bytes_read;
}

int main(int argc, char **argv) {
    int fbfd = -1;              /* Framebuffer file descriptor */
    int sockfd;                 /* TCP socket file descriptor */
    struct sockaddr_in servaddr; /* Server address structure */
    unsigned char buffer[WIDTH * HEIGHT * 2];  /* Buffer for YUYV data */

/* Framebuffer initialization: open -> ioctl -> mmap */
    /* Open the framebuffer device */
    fbfd = open(FBDEV, O_RDWR);
    if(fbfd == -1) {
        perror("Error opening framebuffer device");
        return EXIT_FAILURE;
    }

    /* Get framebuffer information */
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading variable information.");
        return EXIT_FAILURE;
    }

    /* Memory map the framebuffer */
    long screensize = vinfo.xres * vinfo.yres * 2;
    fbp = (short *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if(fbp == (short*)-1) {
        perror("Error mapping framebuffer to memory");
        return EXIT_FAILURE;
    }

/* TCP connection: socket -> connect */
    /* Create a TCP socket */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    /* Set up the server address */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(TCP_PORT);
    servaddr.sin_addr.s_addr = inet_addr("Server IP Address");  // Replace with actual server IP address

    /* Connect to the server */
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection to the server failed");
        return EXIT_FAILURE;
    }

    /* Receive YUYV data from the server and render it */
    while(1) {
        /* Ensure full frame data is received */
        if(read_full_data(sockfd, buffer, sizeof(buffer)) <= 0) {
            break;  // Exit the loop on error or disconnection
        }

        /* Convert received YUYV data to RGB and display it on the framebuffer */
        process_image(buffer);
    }

    /* Clean up resources */
    munmap(fbp, screensize);
    close(fbfd);
    close(sockfd);

    return 0;
}
