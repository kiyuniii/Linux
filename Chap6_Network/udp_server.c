#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define UDP_PORT 5100

int main(int argc, char **argv) {
    int sockfd, n;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char mesg[1000];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    /* 접속되는 클라이언트를 위한 주소설정 후 운영체제에 서비스 등록 */
    /* servaddr을 바이트 조작, 0 = 초기화 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;                //주소체계: AF_INET
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //주소설정 = 엔디안변환(INADDR_ANY), (4byte, long)
    servaddr.sin_port = htons(UDP_PORT);          //포트설정 = (2byte, short)
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    /* 클라이언트로부터 메시지를 받아 다시 전송 */
    do {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, mesg, sizeof(mesg), 0, (struct sockaddr *)&cliaddr, &len);
        sendto(sockfd, mesg, sizeof(mesg), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr)); //서버에 메시지 보낸 클라이언트에게 그대로 다시 보냄

        mesg[n] = '\0';
        printf("Received data: %s\n", mesg);
    } while(strncmp(mesg, "q", 1)); //q를 보내면 종료(q를 할 때까지 반복

    close(sockfd);
    return 0;
}



