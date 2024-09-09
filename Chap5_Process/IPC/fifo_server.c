/* 필요한 헤더 파일 포함 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/* FIFO 파일 이름 정의 */
#define FIFOFILE "fifo"

/* 메인 함수 */
int main(void) {
    int n, fd;
    char buf[BUFSIZ];

    /* FIFO 파일 생성 */
    if(mkfifo(FIFOFILE, 0666) < 0) {
        perror("mkfifo()");
        return -1;
    }
    
    /* FIFO 파일 열기 */
    if((fd = open(FIFOFILE, O_RDONLY)) < 0) {  // 읽기 전용으로 열기
        perror("open()");
        return -1;
    }

    /* FIFO에서 데이터 읽기 및 출력 */
    while((n = read(fd, buf, BUFSIZ)) > 0)  // 데이터를 읽어 버퍼에 저장
        printf("%s", buf);  // 읽은 데이터 출력

    /* 파일 디스크립터 닫기 */
    close(fd);
    return 0;
}