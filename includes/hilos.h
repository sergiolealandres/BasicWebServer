/**********************************
 * @author Pablo Soto y Sergio Leal
 * Grupo 2301
 * Redes de Comunicaciones II
 * Práctica 1:Servidor Web
 * Curso 21/22
 * *******************************/

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

#include "../includes/lib_socketlib.h"
#include "../includes/procesar.h"

#define MAX_SIZE 2048

/**
 * @brief Estructura usada para pasar como parámetro 
 * de la función threas_main y thread_make
 * @param i número de hilo
 * @param server_root path con el root del servidor
 * @param server_signature nombre y versión del servidor
 * @param socketid puntero al socket de la conexión gestionada por el hilo
 * @param thread_id id del hilo
 * @param thread_count número de hilos
 * 
 */
typedef struct{
    int i;
    char *server_root;
    char *server_signature;
    int socketid;
    pthread_t thread_tid;
    long thread_count;
    
}HiloArg;


/**
 * @brief Variables globales compartidas utilizadas por los hilos
 * 
 */
int listenfd, nthreads;
pthread_mutex_t mlock;

/**
 * @brief Función que contiene la funcionalidad del hilo.
 * Inicializa los manejadores de señales del hilo, acepta conexiones 
 * al servidor usando un mutex para que no haya dos accept concurrentes
 * y procesa las peticiones que le llegan
 * 
 * @param h HiloArg con los argumentos del hilo
 */
void thread_make(HiloArg *h);

/**
 * @brief función que se llama para lanzar los hilos
 * Llama a thread_make que incluye toda la funcionalidad de los hilos
 * 
 * @param arg 
 * @return void* 
 */
void * thread_main(void *arg);



#endif
