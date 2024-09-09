#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fb.h>

#define FB "/dev/fb0"
#define COL 640

typedef unsigned char ubyte;

unsigend short makepixel(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned short)(((r>>3)<<11 | ((g>>2)<<5 | (b>>3));
