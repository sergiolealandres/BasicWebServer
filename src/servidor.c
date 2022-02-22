#include "../includes/lib_socketlib.h"


#include "../includes/hilos.h"

pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;

void sig_int(int signal){

}

int main(int argc, char **argv){

    int i;

    listenfd=initiate_server();
    nthreads = 10;
    tptr = calloc(nthreads, sizeof(Thread));
    printf("hoola2\n");
    for (i = 0; i < nthreads; i++) thread_make(i);
    printf("salgo del for\n");
    //signal(SIGINT, sig_int);

    for ( ; ; )pause();

}
