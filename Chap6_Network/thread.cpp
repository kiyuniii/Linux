#include <iostream>
#include <thread>   //#include <pthread.h>

using namespace std;

void call_from_thread(int tid) {
    cout << "스레드 실행 : " << tid << endl;
    std::this_thread::sleep_for(std::chrono::nanoseconds(tid*20));
}

int main() {
    thread t[10];

    /* 10개의 스레드 시작 */
    thread(call_from_thread, 0).detach();
    for(int i=1; i<10; i++) {
        t[i] = thread(call_from_thread, i);
    }

    cout << "메인 함수 시작 : " << endl;

    /* 스레드가 종료할 때 까지 대시 */
    for(int i=1; i<10; i++) {
        if(t[i].joinable())
            t[i].join();
    }

    return 0;
}
/* 
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* call_from_thread(void* arg) {
    int tid = *((int*)arg);
    printf("스레드 실행 : %d\n", tid);
    usleep(tid * 20);  // microsecond 단위로 sleep
    return NULL;
} */

/* C언어 코드 */
/* int main() {
    pthread_t t[10];
    int ids[10];  // 스레드에 넘길 ID를 저장할 배열

    // 10개의 스레드 시작
    ids[0] = 0;
    pthread_t t0;
    pthread_create(&t0, NULL, call_from_thread, &ids[0]);
    pthread_detach(t0);  // 메인 스레드에서 join을 기다리지 않음

    for (int i = 1; i < 10; i++) {
        ids[i] = i;
        pthread_create(&t[i], NULL, call_from_thread, &ids[i]);
    }

    printf("메인 함수 시작 : \n");

    // 스레드가 종료할 때까지 대기
    for (int i = 1; i < 10; i++) {
        pthread_join(t[i], NULL);
    }

    return 0;
}
 */