#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static void printSigset(sigset_t *set); //시그널집합
static void sigHandler(int);            //(시그널)핸들러

int main(int argc, char **argv) {
    sigset_t pset;      //시그널집합 정의
    sigemptyset(&pset);         //시그널 비움
    sigaddset(&pset, SIGQUIT);  //시그널집합 추가(SIGQUIT)
    sigaddset(&pset, SIGRTMIN); //시그널집합 추가(SIGRTMIN)
    sigprocmask(SIG_BLOCK, &pset, NULL);    //현재 프로세스에서 시그널집합을 블록(SIGQUIT, SIGRTMIN)

    printSigset(&pset);     //시그널집합 출력
    
    /* 시그널(SIGINT)을 처리할 핸들러 연결 */
    if(signal(SIGINT, sigHandler) == SIG_ERR) {
        perror("signal() : SIGINT");
        return -1;
    }

    /* 시그널(SIGUSR1)을 처리할 핸들러 연결 */
    /* SIGUSR1 = 사용자 정의 시그널 */
    if(signal(SIGUSR1, sigHandler) == SIG_ERR) {
        perror("signal() : SIGUSR1");
        return -1;
    }

    /* 시그널(SIGUSR2)을 처리할 핸들러 연결 */
    if(signal(SIGUSR2, sigHandler) == SIG_ERR) {
        perror("signal() : SIGUSR2");
        return -1;
    }
    
    /* 시그널(SIGPIPE)을 무시(SIG_IGN)하도록 설정 */
    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal() : SIGPIPE");
        return -1;
    }

    /* 무한루프 : 시그널을 기다리는 동안 프로세스는 대기 */
    while(1) pause();
    return 0;
}

/* (시그널)핸들러에 대한 정의 */
static void sigHandler(int signo) { //시그널 번호를 매개변수로 받음
    if(signo == SIGINT) {
        printf("SIGINT is catched : %d\n", signo);
        exit(0);
    } else if(signo == SIGUSR1) {
        printf("SIGUSR1 is catched\n");
    } else if(signo == SIGUSR2) {
        printf("SIGUSR2 is catched\n");
    } else if(signo == SIGQUIT) {
        printf("SIGQUIT is catched\n");

        /* 시그널(SIGINT)를 블록해제 하기 위한 새로운 시그널집합 선언 */
        sigset_t uset;
        sigemptyset(&uset);
        sigaddset(&uset, SIGINT);   //시근러(SIGINT)를 시그널집합에 추가
        sigprocmask(SIG_UNBLOCK, &uset, NULL);  //UNBLOCK하여 시그널마스크에서 제거
    } else {
        fprintf(stderr, "Catched signal : %s\n", strsignal(signo));
    }
}

/* 시그널집합을 출력하는 함수 */
static void printSigset(sigset_t *set) {
    int i;
    for(i=0; i<NSIG; ++i) {
        /* 집합에 포함된 시그널: 1, 포함되지 않은 시그널: 0 */
        printf((sigismember(set, i))?"1":"0");
    }
    putchar('\n');
}
