#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT 5100

int main(int argc, char **argv) {
    int ssock, csock;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[BUFSIZ];

    if(argc < 2) {
        printf("usage: %s [IP address]\n", argv[0]);
        return -1;
    }

    /* 소켓 생성 */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    /* 주소 설정 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    /* 문자열(argv[1])을 네트워크주소로 변환 */
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr);
    /* 포트설정 */
    servaddr.sin_port = htons(TCP_PORT);

    /* 지정한 주소로 접속 */
    if(connect(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {    //server의 accept와 매개변수 똑같음
        perror("connect()");
        return -1;
    }

    fgets(mesg, BUFSIZ, stdin);
    if(send(ssock, mesg, BUFSIZ, MSG_DONTWAIT) <= 0) {
        perror("send()");
        return -1;
    }

    memset(mesg, 0, BUFSIZ);
    if(recv(ssock, mesg, BUFSIZ, 0) <= 0) {
        perror("recv()");
        return -1;
    }
    printf("Received data: %s\n", mesg);

    close(ssock);
    return 0;
}