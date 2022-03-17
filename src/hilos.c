#include "../includes/hilos.h"

void sig_thread_handler(int sig){
    pthread_exit(NULL);

}

void sig_pipe1(int signal){
    
}


void * thread_main(void *arg){
    HiloArg *h;
    int connfd=-1;

    h = (HiloArg*) arg;
    struct sigaction act;

    sigemptyset(&(act.sa_mask));
    act.sa_flags=0;
    act.sa_handler=sig_thread_handler;

    //Añadimos un manejador para asegurarnos del cierre y salida correcta de los hilos
    sigaction(SIGUSR2, &act, NULL);

    act.sa_handler=sig_pipe1;

    //Ignoramos la señal SIGPIPE
    sigaction(SIGPIPE, &act, NULL);
    
    
    while (1){
        

        pthread_mutex_lock(&mlock);

        connfd = accept_connection(listenfd);
        *(h->socketid) = connfd;
        
        pthread_mutex_unlock(&mlock);//Levanta el semáforo para que pase otro hilo

        
        procesar_conexion(connfd,h->server_root,h->server_signature);//Procesa la petición que ha recibido

        close(connfd);//Cierra el socket particular que había abierto para esa conexión
        *(h->socketid) = connfd;

    }
    //En caso de que le llegue la señal SIGUSR2 en un momento diferente al down del
    //mutex o el accept también debe de salir correctamente

}

void thread_make(HiloArg *h){
    //Función auxiliar para lanzar los hilos
    pthread_create(&tptr[h->i].thread_tid, NULL, &thread_main, (void *) h);  
    
}
