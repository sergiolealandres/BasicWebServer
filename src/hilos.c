#include "../includes/hilos.h"
static volatile int got_signal=0;


void sig_thread_handler(int sig){
    got_signal=1;
    printf("señal recibida\n");
}


void * thread_main(void *arg){
    HiloArg *h;
    int connfd;
    Request *request=request_create();
    Request *copia;
    socklen_t clilen;
    struct sockaddr *cliaddr;
    cliaddr = malloc(addrlen);

    h = (HiloArg*) arg;
    struct sigaction act;

    sigemptyset(&(act.sa_mask));
    act.sa_flags=0;
    act.sa_handler=sig_thread_handler;

    sigaction(SIGUSR2, &act, NULL);


    //printf("thread %d starting\n", (int) arg);
    while (got_signal==0){
        clilen = addrlen;
        //printf("hoola222\n");
        //alarm(2);
        while(pthread_mutex_lock(&mlock)){
            if(errno != EINTR){
            perror("sem_wait");
            return 0;
        }

        if(got_signal==1){
            

            break;
        }
        }
        //printf("soy koke\n");
         if(got_signal==1){
            //printf("salgo\n");
            request_free(request);
            //printf("salgo\n");
            pthread_exit(NULL);

        }
        //printf("soy koke\n");
        connfd = accept_connection(listenfd);
        //printf("soy kokepost accept\n");
         if(got_signal==1){
            //printf("salgo\n");
            request_free(request);
            //printf("salgo\n");
            pthread_exit(NULL);

        }
        
        //printf("soy koke\n");
        //printf("in mutex\n");
        pthread_mutex_unlock(&mlock);
        //tptr[*((int*) arg)].thread_count++;
        procesar_conexion(connfd,h->server_root,h->server_signature);
        //printf("hoola2\n");
    
        //launch_service(connfd);
        close(connfd);
    }
    if(got_signal==1){
        //printf("salgo\n");
        request_free(request);
        //printf("salgo\n");
        pthread_exit(NULL);
    }
/* process request */
}
void thread_make(HiloArg *h){
    printf("thread make da %s y %s\n",h->server_root,h->server_signature);
    pthread_create(&tptr[h->i].thread_tid, NULL, &thread_main, (void *) h);  
    return;
}
