#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdlib.h>

#define FBDEVICE "/dev/fb0"

typedef unsigned char ubyte;
struct fb_var_screeninfo vinfo;

extern inline unsigned short makepixel(unsigned char r, unsigned char g, unsigned b) {
    return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

/* p.184 off_t lseek(int fd, off_t offset, int whence */
static void drawfrancemmap(int fd, int start_x, int start_y, int end_x, int end_y, ubyte r, ubyte g, ubyte b) {
    ubyte *pfb, a = 0xFF;
    int color = vinfo.bits_per_pixel/8.;

    if(end_x == 0) end_x = vinfo.xres;
    if(end_y == 0) end_y = vinfo.yres;

    pfb = (ubyte *)mmap(0, vinfo.xres * vinfo.yres * color, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    for(int x = start_x; x < end_x * color; x+=color) {
        for(int y = start_y; y < end_y; y++) {
            *(pfb + (x+0) + y*vinfo.xres*color) = b;
            *(pfb + (x+1) + y*vinfo.xres*color) = g;
            *(pfb + (x+2) + y*vinfo.xres*color) = r;
            *(pfb + (x+3) + y*vinfo.xres*color) = a;
        }
    }
    munmap(pfb, vinfo.xres * vinfo.yres * color);
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
    
    drawfrancemmap(fbfd, 0, 0, 0, 0, 255, 255, 0);

    /*   
    drawface(fbfd, 0, 0, 50, 150, 0, 0, 255);
    drawface(fbfd, 50, 0, 100, 150, 255, 255, 255);
    drawface(fbfd, 100, 0, 150, 150, 255, 0, 0);
    */
    close(fbfd);

    return 0;
}
