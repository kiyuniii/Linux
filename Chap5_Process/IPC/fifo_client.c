/* 필요한 헤더 파일 포함 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/* FIFO 파일 이름 정의 */
#define FIFOFILE "fifo"

/* 메인 함수 */
int main(int argc, char **argv) {
    int n, fd;
    char buf[BUFSIZ];

    /* FIFO 파일 열기 */
    if((fd = open(FIFOFILE, O_WRONLY)) < 0) {  // 쓰기 전용으로 열기
        perror("open()");
        return -1;
    }
    
    /* 표준 입력에서 데이터를 읽어 FIFO에 쓰기 */
    while((n = read(0, buf, sizeof(buf))) > 0)  // 표준 입력에서 데이터를 읽어 버퍼에 저장
        write(fd, buf, n);  // 버퍼에 저장된 데이터를 FIFO에 쓰기 // 클라이언트에서 서버로 데이터 전송
    
    /* 파일 디스크립터 닫기 및 프로그램 종료 */
    close(fd);  // 파일 디스크립터 닫기
    return 0;  // 프로그램 정상 종료
}