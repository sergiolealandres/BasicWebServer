#include "../includes/hilos.h"
static volatile int got_signal=0;


void sig_thread_handler(int sig){
    got_signal=1;
    printf("señal recibida\n");
    //pthread_exit(NULL);
}


void * thread_main(void *arg){
    HiloArg *h;
    int connfd;
    
    socklen_t clilen;

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
        //printf("waiting in mutex, %d\n",h->i);
        pthread_mutex_lock(&mlock);
        //printf("Justo despues de lock, %d\n",h->i);

        if(got_signal==1){
                
            
            pthread_mutex_unlock(&mlock);
            //printf("salgo despues de lock, %d\n",h->i);
            return NULL;
            pthread_exit(NULL);
        }
        //printf("soy koke\n");
        connfd = accept_connection(listenfd);
        //printf("soy kokepost accept\n");
         if(got_signal==1){
            //printf("salgo del accept ,%d\n",h->i);
            
            pthread_mutex_unlock(&mlock);
            //printf("salgo despues de unlock accept %d,\n",h->i);
            return NULL;
            pthread_exit(NULL);

        }
        
        //printf("soy koke\n");
        //printf("in mutex\n");
        pthread_mutex_unlock(&mlock);
        //printf("Despues del unlock%d, socket es %d\n",h->i,connfd);
        //tptr[*((int*) arg)].thread_count++;
        procesar_conexion(connfd,h->server_root,h->server_signature);
        //printf("hoola2\n");
    
        //launch_service(connfd);
        close(connfd);
        //printf("Al final del bucle, %d\n",h->i);
    }
    if(got_signal==1){
        //printf("salgo fuera del while, %d\n",h->i);
        
        //printf("salgo\n");
        return NULL;
        pthread_exit(NULL);
    }
/* process request */
}
void thread_make(HiloArg *h){
    printf("thread make da %s y %s\n",h->server_root,h->server_signature);
    pthread_create(&tptr[h->i].thread_tid, NULL, &thread_main, (void *) h);  
    return;
}
