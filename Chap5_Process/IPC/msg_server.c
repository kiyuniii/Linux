/* 필요한 헤더 파일 포함 */
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>

/* 메시지 큐 키 정의 */
#define MSQKEY 51234

/* 메시지 버퍼 구조체 정의 */
struct msgbuf {
    long mtype;
    char mtext[BUFSIZ];
};

int main(int argc, char **argv) {
    key_t key;
    int n, msqid;
    struct msgbuf mb;

    /* 메시지 큐 키 설정 */
    key = MSQKEY;
    
    /* 메시지 큐 생성 */
    if((msqid = msgget(key, IPC_CREAT | IPC_EXCL | 0666)) < 0) {
        perror("msgget()");
        return -1;
    }

    /* 메시지 수신 및 처리 루프 */
    while((n = msgrcv(msqid, &mb, BUFSIZ, 0, 0)) > 0) {
        switch(mb.mtype) {
            case 1:
                write(1, mb.mtext, n);  // 메시지 내용을 표준 출력에 쓰기
                break;
            case 2:
                /* 메시지 큐 제거 */
                if(msgctl(msqid, IPC_RMID, (struct msqid_ds *)0) < 0) {
                    perror("msgctl()");
                    return -1;
                }
                break;
        }
    }
    return 0;
}
