/**********************************
 * @author Pablo Soto y Sergio Leal
 * Grupo 2301
 * Redes de Comunicaciones II
 * Práctica 1:Servidor Web
 * Curso 21/22
 * *******************************/

#ifndef PROCESAR
#define PROCESAR

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <syslog.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include "../includes/picohttpparser.h"

#define MAX_HEADER 4096
#define NOMBRE_SERVIDOR "Elpeonkamikaze"
#define VERSION_SERVIDOR "1.0"
#define ALLOWS "OPTIONS, GET, POST, HEAD"
#define MAX_ANSWER 131072
#define MAX_PATH 2048
#define ONE_KB 1024
#define SMALL_SIZE 128

/**
 * @brief Estructura donde se guardan los datos de una petición parseada
 * @param method método pedido
 * @param method_len longitud del método
 * @param path path del fichero que se solicta
 * @param path_len longitud del path
 * @param headers Array con los distintos campos de la cabecera
 * @param num_headers Número de campos
 * @param minor_version Versión HTTP usada
 * @param buf Guarda el cuerpo de la petición
 * @param buflen Longitud del cuerpo
 * 
 */
typedef struct {

    char *method;
    size_t method_len;
    char *path;
    size_t path_len;
    struct phr_header headers[SMALL_SIZE];
    size_t num_headers;
    int minor_version;
    char buf[MAX_HEADER];
    size_t buflen;


} Request;

/**
 * @brief Parsea una petición. Para ello llama a read() hasta que llega una petición,
 * y mediante picohttpparser se obtienen los distintos campos de la petición que se 
 * guardan en una Request.
 * 
 * @param socketfd socket de la conexión
 * @param request doble puntero a la petición parseada
 * @return int 
 */
int parsear_peticion(int socketfd, Request *request);

/**
 * @brief Procesa una petición. Para ello llama parsear_peticion y en función de
 *  estos se llama a otras funciones de procesado.
 * 
 * @param socketfd socket de la conexión
 * @param server_root root_del servidor
 * @param server_signature nombre y versión del servidor
 * @return int 0 si no hay errores, -1 en el caso contrario
 */
int procesar_conexion(int socketfd,char *server_root, char *server_signature);

/**
 * @brief Construye una cabecera para una respuesta HTTP. 
 * 
 * @param codigo codigo de respuesta
 * @param path_recurso path del fichero del que se hace una respuesta (NULL si no hay fichero)
 * @param server_signature nombre y versión del servidor
 * @param flagOptions opciones de cabecera. Se explica con más detalle en la wiki en la sección Enviar respuestas
 * @return char* String con la cabecera de la respuesta
 */
char * construir_cabecera(char *codigo,char *path_recurso,char *server_signature, int flagOptions);

/**
 * @brief Ejecuta el comando dado como parámetro y manda la salida por la conexión dada en socketfd
 * 
 * @param socketfd conexión en la que mandar la respuesta
 * @param comando comando a ejecutar
 * @param server_signature nombre y versión del servidor
 * @return int 
 */
int executeAndPrintOnScreen(int socketfd, char*comando,char* server_signature);


/**
 * @brief manda una respuesta por el socket de una conexión
 * 
 * @param socketfd socket por el que se manda la respuesta
 * @param codigo codigo de respuesta
 * @param path path del fichero del que se hace una respuesta (NULL si no hay fichero)
 * @param server_signature nombre y versión del servidor
 * @param flagOptions opciones de repuesta. Se explica con más detalle en la wiki en la sección Enviar respuestas
 */
void mandar_respuesta(int socketfd,char *codigo,char *path,char *server_signature, int flagOptions);


/**
 * @brief FUnción que implementa la funcionalidad GET
 * 
 * @param socketfd socket de la conexión
 * @param r Petición del GET
 * @param server_root root del servidor
 * @param server_signature nombre y versión del servidor
 */
void get(int socketfd, Request r,char *server_root,char *server_signature);

/**
 * @brief Función que implementa la funcionalidad POST
 * 
 * @param socketfd socket de la conexión
 * @param r Petición del POST
 * @param server_root root del servidor
 * @param server_signature nombre y versión del servidor
 */
void post(int socketfd, Request r,char *server_root,char *server_signature);

/**
 * @brief Función que implementa la funcionalidad OPTIONS
 * 
 * @param socketfd socket de la conexión
 * @param r Petición de OPTIONS
 * @param server_signature nombre y versión del servidor
 */
void options(int socketfd, Request request, char *server_signature);

/**
 * @brief Función que implementa la funcionalidad HEAD
 * 
 * @param socketfd socket de la conexión
 * @param r Petición del HEAD
 * @param server_signature nombre y versión del servidor
 */
void head(int socketfd, Request r, char *server_root, char *server_signature);

/**
 * @brief Función que convierte un comando por URL a un comando ejecutable
 * 
 * @param dirtypath 
 * @param cleanpath
 */
void clean_path_get(char **dirtypath, char*cleanpath);

/**
 * @brief Busca el campo Content Length de una petición
 * 
 * @param r petición
 * @param position Posición en la que se encuentra Content Length
 * @return int 
 */
int searchForContentLength(Request r, int *position);

#endif
