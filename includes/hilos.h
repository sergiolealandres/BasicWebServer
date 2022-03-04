#ifndef HILOS
#define HILOS

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "../includes/procesar.h"
#include "../includes/lib_socketlib.h"

#define MAX_SIZE 2048

typedef struct{
    int i;
    char *server_root;
    char *server_signature;
}HiloArg;


typedef struct {

pthread_t thread_tid;

long thread_count;
} Thread;

Thread *tptr;


int listenfd, nthreads;
socklen_t addrlen;
pthread_mutex_t mlock;


void thread_make(HiloArg *arg);


void * thread_main(void *arg);
#endif