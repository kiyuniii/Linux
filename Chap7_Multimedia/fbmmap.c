#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <linux/fb.h>

#define FBDEVFILE "/dev/fb0"

#define XPOS1 0
#define YPOS1 0
#define XPOS2 640
#define YPOS2 480

typedef unsigned char ubyte;

unsigned short makepixel(ubyte r, ubyte g, ubyte b){
	return (unsigned short)(((r>>3)<<11)|((g>>2)<<6)|(b>>3));
}
int main(int argc, char *argv[])
{
	int fd;
	int offset,t,tt;
	unsigned short pixel;
	unsigned short *pfbdata;
	unsigned char r, g, b;
	
	if(argc !=4){
		printf("Please input the parameter ! ex)./color R G B\n");
		return -1;
	}
	
	r = atoi(argv[1]);
	g = atoi(argv[2]);
	b = atoi(argv[3]);
	
	fd = open(FBDEVFILE, O_RDWR);
	if(fd<0){
		printf("Error fbdev open\n");
		exit(1);
	}
	
	pfbdata = (unsigned short *)mmap(0, XPOS2*YPOS2*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if((unsigned)pfbdata == (unsigned)-1){
		printf("Error fbdev mmap\n");
		exit(1);
	}
	
	pixel = makepixel(r,g,b);
	
	for(t=YPOS1; t<YPOS2; t++){
		offset=t*XPOS2;
		for(tt=XPOS1; tt<XPOS2; tt++){
			*(pfbdata + offset + tt) = pixel;
			// lseek(fd, offset, SEEK_SET);
			// write(fd, &pixel, 2);			
		}
	}
	
	munmap(pfbdata, XPOS2*YPOS2*2);
	close(fd);
	return 0;
}
