/* 파이프를 이용한 프로세스 간 통신 예제 */

#include <stdio.h>      // 표준 입출력 함수를 사용하기 위한 헤더
#include <unistd.h>     // 유닉스 표준 함수를 사용하기 위한 헤더
#include <sys/wait.h>   // 자식 프로세스 대기 함수를 사용하기 위한 헤더

int main(int argc, char **argv) {
    pid_t pid;          // 프로세스 ID를 저장할 변수
    int pfd[2];         // 파이프의 파일 디스크립터를 저장할 배열
    char line[BUFSIZ];  // 데이터를 읽어올 버퍼
    int status;         // 자식 프로세스의 종료 상태를 저장할 변수

    /* 파이프 생성 */
    if(pipe(pfd) < 0) {  // 파이프 생성
        perror("pipe()");  // 파이프 생성 실패 시 에러 메시지 출력
        return -1;         // 프로그램 종료
    }

    /* 프로세스 복제 */
    if((pid = fork()) < 0) {  // 프로세스 복제
        perror("fork()");     // 프로세스 복제 실패 시 에러 메시지 출력
        return -1;            // 프로그램 종료
    } else if(pid == 0) {     // 자식 프로세스인 경우
        /* 자식 프로세스: date 명령어 실행 및 결과를 파이프에 쓰기 */
        close(pfd[0]);        // 읽기용 파이프 닫기
        dup2(pfd[1], 1);      // 표준 출력을 파이프의 쓰기용으로 복제
        execl("/bin/date", "date", NULL);  // date 명령어 실행
        close(pfd[1]);        // 쓰기용 파이프 닫기
        _exit(127);           // 자식 프로세스 종료
    } else {                  // 부모 프로세스인 경우
        /* 부모 프로세스: 파이프에서 데이터 읽기 및 출력 */
        close(pfd[1]);        // 쓰기용 파이프 닫기
        if(read(pfd[0], line, BUFSIZ) < 0) {  // 파이프에서 데이터 읽기
            perror("read()");  // 읽기 실패 시 에러 메시지 출력
            return -1;         // 프로그램 종료
        }
        printf("%s", line);    // 읽은 데이터 출력

        close(pfd[0]);         // 읽기용 파이프 닫기
        waitpid(pid, &status, 0);  // 자식 프로세스가 종료될 때까지 대기
    }
    return 0;  // 프로그램 정상 종료
}