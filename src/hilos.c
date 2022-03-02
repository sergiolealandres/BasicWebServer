#include "../includes/procesar.h"
#include "../includes/hilos.h"
#include "../includes/lib_socketlib.h"
static volatile int got_signal=0;


void sig_thread_handler(int sig){
    got_signal=1;
}


void * thread_main(void *arg){
    int connfd;
    Request *request=request_create();
    Request *copia;
    socklen_t clilen;
    struct sockaddr *cliaddr;
    cliaddr = malloc(addrlen);
    signal(SIGUSR2, sig_thread_handler);
    //printf("thread %d starting\n", (int) arg);
    while (got_signal==0){
        clilen = addrlen;
        printf("hoola222\n");
        pthread_mutex_lock(&mlock);

         if(got_signal==1){
            
            request_free(request);
            pthread_exit(NULL);

        }

        connfd = accept_connection(listenfd);

         if(got_signal==1){
            
            request_free(request);
            pthread_exit(NULL);

        }

        printf("in mutex\n");
        pthread_mutex_unlock(&mlock);
        //tptr[*((int*) arg)].thread_count++;
        procesar_conexion(connfd);
        printf("hoola2\n");
    
        //launch_service(connfd);
        close(connfd);
    }
    if(got_signal==1){
        request_free(request);
        pthread_exit(NULL);
    }
/* process request */
}
void thread_make(int i){

    pthread_create(&tptr[i].thread_tid, NULL, &thread_main, (void *) &i); 
    return;
}
