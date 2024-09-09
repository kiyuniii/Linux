#include <stdio.h>

#include "bmpHeader.h"

int readBmp(char *filename, unsigned char **data, int *cols, int *rows) {
    BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER bmpInfoHeader; 
    FILE *fp;

    /* bmp 파일을 오픈한다. */
    fp = fopen(filename, "rb");
    if(fp == NULL) {
        perror("ERROR\n");
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    /* 트루 컬러를 지원하지 않으면 표시할 수 없음 */
    if(bmpInfoHeader.biBitCount != 24) {
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp)  ;
        return -1;
    }

    /* 이미지의 해상도의 정보를 가져온다. */
    *cols = bmpInfoHeader.biWidth;
    *rows = bmpInfoHeader.biHeight;

    /* 이미지의 해상도의 정보를 가져온다. */
    printf("Resolution: %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count: %d\n", bmpInfoHeader.biBitCount);

    /* 실제 이미지 데이터가 있는 위치를 계산해서 가져온다. */
    fseek(fp, bmpHeader.bfOffBits, SEEK_SET);
    fread(*data, 1, bmpHeader.bfSize - bmpHeader.bfOffBits, fp);

    fclose(fp);
    return 0;
}
