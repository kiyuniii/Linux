/* 헤더 파일 포함 및 상수 정의 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#include "bmpHeader.h"

#define FBDEVFILE "/dev/fb0"
#define LIMIT_UBYTE(n) (n>UCHAR_MAX?UCHAR_MAX:(n<0)?0:n)

typedef unsigned char ubyte;

/* 함수 선언 */
extern int readBmp(char *filename, ubyte **pData, int *cols, int *rows, int *color);

/* 24bit -> 16bit */
extern inline unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b) {
    return (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3);
}

/* 메인 함수 */
int main(int argc, char **argv) {
    /* 변수 선언 */
    int cols, rows, color=24;   
    ubyte r, g, b, a=255;
    ubyte *pData, *pBmpData, *pFbMap;
    struct fb_var_screeninfo vinfo;
    int fbfd;

    /* 명령행 인자 확인 */
    if(argc != 2) {
        printf("Usage: ./%s xxx.bmp\n", argv[0]);
        return 0;
    }

    /* 프레임 버퍼 초기화 */
    fbfd = open(FBDEVFILE, O_RDWR);
    if(fbfd < 0) {
        perror("open()");
        return -1;
    }

    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) > 0) {
        perror("ioctl() : FBIOGET_VSCREENINFO");
        return -1;
    }
    
    /* 메모리 할당 */
    pBmpData = (ubyte *)malloc(vinfo.xres * vinfo.yres * sizeof(ubyte) * vinfo.bits_per_pixel/8);
    pData = (ubyte *) malloc(vinfo.xres * vinfo.yres * sizeof(ubyte) * color/8);

    pFbMap = (ubyte *)mmap(0, vinfo.xres * vinfo.yres * vinfo.bits_per_pixel/8, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);
    if(pFbMap == MAP_FAILED) {
        perror("mmap()");
        return -1;
    }

    /* BMP 파일 읽기 */
    if(readBmp(argv[1], &pData, &cols, &rows, &color) < 0) {
        perror("readBmp()");
        return -1;
    }

    /* BMP 이미지 데이터를 프레임 버퍼 형식으로 변환 */
    for(int y=0, k, total_y; y < rows; y++) {
        k = (rows-y-1) * cols * color/8; // BMP 이미지는 상하가 뒤집혀 있어 역순으로 처리
        total_y = y * vinfo.xres * vinfo.bits_per_pixel/8;

        for(int x = 0; x < cols; x++) {
            b = LIMIT_UBYTE(pData[k + x*color/8 + 0]); // 파란색 값 추출
            g = LIMIT_UBYTE(pData[k + x*color/8 + 1]); // 초록색 값 추출
            r = LIMIT_UBYTE(pData[k + x*color/8 + 2]); // 빨간색 값 추출

            unsigned short pixel = makepixel(r, g, b); // 24비트를 16비트로 변환
            *(unsigned short *)(pBmpData + x*2 + total_y) = pixel; // 16비트 픽셀 저장


/*             *(pBmpData + x*vinfo.bits_per_pixel/8 + total_y + 0) = b;
            *(pBmpData + x*vinfo.bits_per_pixel/8 + total_y + 1) = g;
            *(pBmpData + x*vinfo.bits_per_pixel/8 + total_y + 2) = r;
            *(pBmpData + x*vinfo.bits_per_pixel/8 + total_y + 3) = a; // 알파 채널 설정 */
        }
    }

    /* 변환된 이미지 데이터를 프레임 버퍼에 복사 */
    memcpy(pFbMap, pBmpData, vinfo.xres * vinfo.yres * vinfo.bits_per_pixel/8);

    /* 자원 해제 및 정리 */
    munmap(pFbMap, vinfo.xres * vinfo.yres * vinfo.bits_per_pixel/8);
    free(pBmpData);
    free(pData);

    close(fbfd);
    return 0;
}
