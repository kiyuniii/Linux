#define _POSIX_SOURCE
#define _POSIX_C_SOURCE 1
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static void printSigset(sigset_t *set);
static void sigHandler(int);

int main(int argc, char **argv) {
    sigset_t pset;
    sigemptyset(&pset);
    sigaddset(&pset, SIGQUIT);
    sigaddset(&pset, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &pset, NULL);

}