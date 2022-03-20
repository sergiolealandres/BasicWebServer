/**********************************
 * @author Pablo Soto y Sergio Leal
 * Grupo 2301
 * Redes de Comunicaciones II
 * Práctica 1:Servidor Web
 * Curso 21/22
 * *******************************/

#include "../includes/hilos.h"

void sig_thread_handler(int sig){
    
    pthread_exit(NULL);

}

void sig_pipe1(int signal){
    
}


void * thread_main(void *arg){
    HiloArg *h;

    h = (HiloArg*) arg;
    struct sigaction act;

    sigemptyset(&(act.sa_mask));
    act.sa_flags=0;
    act.sa_handler=sig_thread_handler;

    //Añadimos un manejador para asegurarnos del cierre y salida correcta de los hilos
    sigaction(SIGUSR2, &act, NULL);
    
    
    
    while (1){
        

        pthread_mutex_lock(&mlock);//El resto de hilos esperan en el semáforo

        h->socketid = accept_connection(listenfd);//accept
        
        
        pthread_mutex_unlock(&mlock);//Levanta el semáforo para que pase otro hilo

        
        procesar_conexion(h->socketid,h->server_root,h->server_signature);//Procesa la petición que ha recibido
        

        close(h->socketid);//Cierra el socket particular que había abierto para esa conexión
        h->socketid=-1;

    }
    
    //En caso de que le llegue la señal SIGUSR2 en un momento diferente al down del
    //mutex o el accept también debe de salir correctamente

}

void thread_make(HiloArg *h){
    //Función auxiliar para lanzar los hilos
    pthread_create(&h->thread_tid, NULL, &thread_main, (void *) h);  
    
}
