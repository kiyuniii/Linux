/* p.385 
프로토콜 : HTTP
사용기능 : 스레드, 소켓 프로그래밍
함수구성 : 1. MAIN
            - 서버 초기화, 클라이언트 REQ가 들어오면 스레드 생성
         2. CLNT_CONNECTION
         3. SENDDATA
         4. SENDOK
         5. SENDERROR
    - 웹 서버에 스레드를 이용하면 여러 웹 브라우저에 동시접근 가능하다.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* 스레드 처리를 위한 함수 */
static void *clnt_connection(void *arg);
int sendData(FILE* fp, char *ct, char *filename);
void sendOk(FILE* fp);
void sendError(FILE* fp);


/* MAIN
    - 서버 초기화
    - 클라이언트 요청이 들어오면 스레드 생성 */
int main(int argc, char **argv) {
    int ssock;              //Server Socket
    pthread_t thread;
    struct sockaddr_in servaddr, cliaddr;
    unsigned int len;

    /* 프로그램을 시작할 때 서버를 위한 포트번호를 받는다. */
    if(argc != 2) {
        printf("usage: %s <port>\n", argv[0]);
        return -1;
    }

    /* 서버를 위한 소켓을 생성한다. */
    ssock = socket(AF_INET, SOCK_STREAM, 0);
    if(ssock == -1) {
        perror("socket()");
        return -1;
    }

    /* 입력받는 포트번호를 이용해서 서비스를 운영체제에 등록한다. */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = (argc != 2)?htons(8000):htons(atoi(argv[1]));
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind()");
        return -1;
    }

    /* 최대 10개의 클라이언트의 동시접속을 처리할 수 있도록 큐를 생성한다. */
    if(listen(ssock, 10) == -1) {
        perror("listen()");
        return -1;
    }

    while(1) {
        char mesg[BUFSIZ];
        int csock;

        /* 클라이언트의 요청을 기다린다. */
        len = sizeof(cliaddr);
        csock = accept(ssock, (struct sockaddr*)&cliaddr, &len);

        /* 네트워크 주소를 문자열로 변경 */
        inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
        printf("Client IP : %s:%d\n", mesg, ntohs(cliaddr.sin_port));

        /* 클라이언트의 요청이 들어오면 스레드를 생성하고 클라이언트의 요청을 처리한다. */
        pthread_create(&thread, NULL, clnt_connection, &csock);

        /* 연속적인 클라이언트 처리 */
        //pthread_join(thread, NULL);
    }

    return 0;
}

void *clnt_connection(void *arg) {
    /* 스레드를 통해서 넘어온 arg를 int형의 파일 디스크립터로 변환한다. */
    int csock = *((int*)arg);
    FILE *clnt_read, *clnt_write;
    char reg_line[BUFSIZ], reg_buf[BUFSIZ];
    char method[BUFSIZ], type[BUFSIZ];
    char filename[BUFSIZ], *ret;

    /* 파일 디스크립터를 FILE 스트림으로 변환한다. */
    clnt_read = fdopen(csock, "r");
    clnt_write = fdopen(dup(csock), "w");

    /* 한 줄의 문자열을 읽어서 reg_line 변수에 저장한다. */
    fgets(reg_line, BUFSIZ, clnt_read);
    /* reg_line 변수에 문자열을 화면에 출력한다. */
    fputs(reg_line, stdout);
    /* ' ' 문자로 reg_line을 구분해서 요청 라인의 내용(메소드)를 분리한다. */
    ret = strtok(reg_line, "/ ");
    strcpy(method, (ret != NULL)?ret:"");
    if(strcmp(method, "GET") != 0) {
        sendOk(clnt_write);
        goto END;
    } else if(strcmp(method, "GET") != 0) {
        sendError(clnt_write);
        goto END;
    }

    ret = strtok(NULL, " ");
    strcpy(filename, (ret != NULL)?ret:"");
    if(filename[0] == '/') {
        for(int i=0, j=0; i<BUFSIZ; i++, j++) {
            if(filename[0] == '/') j++;
                filename[i] = filename[j];
            if(filename[j] == '\0') break;
        }
    }

    do {
        fgets(reg_line, BUFSIZ, clnt_read);
        fputs(reg_line, stdout);
        strcpy(reg_buf, reg_line);
        char *str = strchr(reg_buf, ':');
    } while(strncmp(reg_line, "\r\n", 2));

    sendData(clnt_write, type, filename);

END:
    fclose(clnt_read);
    fclose(clnt_write);
    pthread_exit(0);
    return (void*)NULL;
}


int sendData(FILE* fp, char *ct, char *filename) {
    /* 클라이언트로 보낼 성공에 대한 응답 메시지 */
    char protocol[ ] = "HTTP/1.1 200 OK\r\n";
    char server[ ] = "Server:Netscape-Enterprise/6.0\r\n";
    char cnt_type[ ] = "Content-Type:text/html\r\n";
    char end[ ] = "\r\n";   //응답헤더의 끝은 항상 \r\n
    char buf[BUFSIZ];
    int fd, len;

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_type, fp);
    fputs(end, fp);

    fd = open(filename, O_RDWR);
    do {
        len = read(fd, buf, BUFSIZ);
        fputs(buf, fp);
    } while(len == BUFSIZ);

    close(fd);
    return 0;
}

void sendOk(FILE* fp) {
    /* 클라이언트에 보낼 성공에 대한 HTTP 응답 메시지 */
    char protocol[ ] = "HTTP/1.1 200 OK\r\n";
    char server[ ] = "Server: Netscape-Enterprise/6.0-\r\n\r\n";
    fputs(protocol, fp);
    fputs(server, fp);
    fflush(fp);
}


void sendError(FILE* fp) {
    /* 클라이언트로 보낼 실패에 대한 HTTP 응답 메시지 */
    char protocol[ ] = "HTTP/1.1 400 Bad Request\r\n";
    char server[ ] = "Server: Netscape-Enterprise/6.0-\r\n\r\n";
    char cnt_len[ ] = "Content-Length:1024\r\n";
    char cnt_type[ ] = "COntent-Type:text/html\r\n\r\n";

    /* 화면에 표시될 HTML 내용 */
    char content1[ ] = "<html><head><title>BAD Connection</title></head>";
    char content2[ ] = "<body><font size=+5>Bad Request</font></body></html>";

    printf("send_error\n");
    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);
    fputs(content1, fp);
    fputs(content2, fp);
    fflush(fp);
}

