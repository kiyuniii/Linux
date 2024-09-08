#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define TCP_PORT 5100

int main(int argc, char **argv) {
    int ssock;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[BUFSIZ];
    socklen_t clen;

    /* 소켓  */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    /* 주소 구조체 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   
    servaddr.sin_port = htons(TCP_PORT);                   
    /* bind */
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()");
        return -1;
    }

    /* 대기 큐 */
    if(listen(ssock, 8) < 0) {
        perror("listen()");
        return -1;
    }

    do {
        int n;
        clen = sizeof(cliaddr);
        int csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);

        /* 네트워크주소(n) -> 문자열(p) */
        inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);    //mesg: client주소
        printf("Client(%s) is connected.\n", mesg);

        if(n = read(csock, mesg, BUFSIZ) < 0)                   //mesg: 받은 data
            perror("read()");
        printf("Received data : %s", mesg);

        /* 클라이언트로 받은 mesg 그대로 다시 전송(echo) */
        if(write(csock, mesg, BUFSIZ) < 0) 
            perror("write()");
        
        close(csock);
    } while(strncmp(mesg, "q", 1));

    close(ssock);
    return 0;
}