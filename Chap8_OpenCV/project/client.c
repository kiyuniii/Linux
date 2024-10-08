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

#define FBDEV       "/dev/fb0"      /* 프레임 버퍼를 위한 디바이스 파일 */
#define VIDEODEV    "/dev/video0"
#define WIDTH       800               /* 캡쳐받을 영상의 크기 */
#define HEIGHT      600
#define TCP_PORT    5100

static short *fbp   	      = NULL; 
static struct fb_var_screeninfo vinfo; 

/* unsigned char의 범위를 넘어가지 않도록 경계 검사를 수행다. */
extern inline int clip(int value, int min, int max)
{
    return(value > max ? max : value < min ? min : value);
}


static void process_image(const void *p)
{
    unsigned char* in =(unsigned char*)p;
    int width = WIDTH;
    int height = HEIGHT;
    int istride = WIDTH*2;          /* 이미지의 폭을 넘어가면 다음 라인으로 내려가도록 설정 */
    int x, y, j;
    int y0, u, y1, v, r, g, b;
    unsigned short pixel;
    long location = 0;
    for(y = 0; y < height; ++y) {
        for(j = 0, x = 0; j < vinfo.xres * 2; j += 4, x += 2) {
            if(j >= width*2) {                 /* 현재의 화면에서 이미지를 넘어서는 빈 공간을 처리 */
                 location++; location++;
                 continue;
            }
            /* YUYV 성분을 분리 */
            y0 = in[j];
            u = in[j + 1] - 128;
            y1 = in[j + 2];
            v = in[j + 3] - 128;

            /* YUV를 RGB로 전환 */
            r = clip((298 * y0 + 409 * v + 128) >> 8, 0, 255);
            g = clip((298 * y0 - 100 * u - 208 * v + 128) >> 8, 0, 255);
            b = clip((298 * y0 + 516 * u + 128) >> 8, 0, 255);
            pixel =((r>>3)<<11)|((g>>2)<<5)|(b>>3);                 /* 16비트 컬러로 전환 */
            fbp[location++] = pixel;

            /* YUV를 RGB로 전환 */
            r = clip((298 * y1 + 409 * v + 128) >> 8, 0, 255);
            g = clip((298 * y1 - 100 * u - 208 * v + 128) >> 8, 0, 255);
            b = clip((298 * y1 + 516 * u + 128) >> 8, 0, 255);
            pixel =((r>>3)<<11)|((g>>2)<<5)|(b>>3);                 /* 16비트 컬러로 전환 */
            fbp[location++] = pixel;
        };
        in += istride;
    };
}

int main(int argc, char **argv) {
    int fbfd = -1;
    int sockfd;
    struct sockaddr_in servaddr;
    unsigned char buffer[WIDTH * HEIGHT * 2];

/* framebuffer : open -> ioctl -> mmap */
    /* 프레임버퍼 열기 */
    fbfd = open(FBDEV, O_RDWR);
    if(-1 == fbfd) {
        perror("open( ) : framebuffer device");
        return EXIT_FAILURE;
    }

    /* 프레임버퍼의 정보 가져오기 */
    if(-1 == ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading variable information.");
        return EXIT_FAILURE;
    }

    /* mmap( ) : 프레임버퍼를 위한 메모리 공간 확보 */
    long screensize = vinfo.xres * vinfo.yres * 2;
    fbp = (short *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if(fbp == (short*)-1) {
        perror("mmap() : framebuffer device to memory");
        return EXIT_FAILURE;
    }

/* TCP -> socket -> connect */
        /* TCP 소켓 생성 */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    /* 서버 정보 설정 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(TCP_PORT);
    servaddr.sin_addr.s_addr = inet_addr("서버 IP 주소");  // 서버의 IP 주소를 입력

    /* 서버에 연결 */
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connect failed");
        return EXIT_FAILURE;
    }

    /* 서버로부터 YUYV 데이터 수신 및 처리 */
    while(1) {
        int bytes_received = read(sockfd, buffer, sizeof(buffer));
        if(bytes_received <= 0) {
            perror("Read error or connection closed");
            break;
        }

        /* 수신한 데이터를 RGB로 변환 후 프레임버퍼에 출력 */
        process_image(buffer);
    }

    /* 자원 해제 */
    munmap(fbp, screensize);
    close(fbfd);
    close(sockfd);

    return 0;
}