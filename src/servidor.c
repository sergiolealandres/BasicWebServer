#include "../includes/lib_socketlib.h"
#include "../includes/procesar.h"
#include "../includes/hilos.h"

pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
static volatile int got_signal=0;

void sig_int(int signal){
    got_signal=1;
}

int main(int argc, char **argv){

    int i;
    listenfd=initiate_server();
    nthreads = 2;
    tptr = calloc(nthreads, sizeof(Thread));
    //printf("hoola2\n");
    for (i = 0; i < nthreads; i++) thread_make(i);
    
    signal(SIGINT, sig_int);

    while(got_signal==0)pause();


    for (i = 0; i < nthreads; i++){
        printf("Cerrando hilo %d...\n", i);
        pthread_kill(tptr[i].thread_tid, SIGUSR2);
        //pthread_join(tptr[i].thread_tid, NULL);
        //printf("OHOOOOO\n");

    }

}
