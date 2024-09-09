#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#define FBDEVICE "/dev/fb0"
typedef unsigned char ubyte;
struct fb_var_screeninfo vinfo;

extern inline unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

static void drawface(int fd, int start_x, int start_y, int end_x, int end_y, ubyte r, ubyte g, ubyte b) {
    ubyte a = 0xFF;

    /* 1600 x 900 */
    //if(end_x == 0) end_x = vinfo.xres;
    //if(end_y == 0) end_y = vinfo.yres;

    for(int x = start_x; x < end_x; x++) {
        for(int y = start_y; y < end_y; y++) {
            int offset = (x + y*vinfo.xres)*vinfo.bits_per_pixel/8.;
            lseek(fd, offset, SEEK_SET);
            unsigned short pixel = makepixel(r, g, b); 
            write(fd, &pixel, 2);
        }
    }
}

int main(int argc, char **argv) {
    int fbfd, status, offset;
    unsigned short pixel;
    
    fbfd = open(FBDEVICE, O_RDWR);
    if(fbfd < 0) {
        perror("Error: cannot open framebuffer device");
        return -1;
    }
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("Error reading fixed information");
        return -1;
    }

    drawface(fbfd, 0, 0, 50, 150, 0, 0, 255);
    drawface(fbfd, 50, 0, 100, 150, 255, 255, 255);
    drawface(fbfd, 100, 0, 150, 150, 255, 0, 0);

    close(fbfd);
    return 0;
}
