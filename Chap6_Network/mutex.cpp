/* 스레드(C/C++) 지원 라이브러리 
- 기존의 스레드 생성은 OS 종속적
    - Windows : W32 Thread
    - UNIX/Linux/MacOS : PThread(POSIX Thread)
- C++11 표준에 스레드 라이브러리 제공
    - std::thread : 스레드 생성/관리
    - std::mutex  : 상호배제 */

#include <iostream>
#include <thread>  
#include <mutex>  

using namespace std;

int value;
mutex value_mutex;

void increase_value() {
    /* 뮤텍스를 이용하여 동기화 */
    value_mutex.lock(); //자원공유 시 발생하는 문제를 제어(lock/unlock)
    value++;
    cout << "스레드 실행 : " << value << endl;
    value_mutex.unlock();
}

int main() {
    thread t[10];

    for(auto i=0; i<10; i++) {
        /* 스레드 생성 */
        t[i] = thread(increase_value);
    }

    for(int i=0; i<10; i++) {
        /* 스레드 종료 */
        t[i].join();
    }
    return 0;
}