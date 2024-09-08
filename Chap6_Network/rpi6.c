#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

static int is_run = 1;
int pressure_fd, temperature_fd;

void* webserverFunction(void* arg);
static void *clnt_connection(void *arg);
int sendData(FILE* fp, char *ct, char *filename);
void sendOk(FILE* fp);
void sendError(FILE* fp);

