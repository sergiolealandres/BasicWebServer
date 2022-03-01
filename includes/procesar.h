#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <syslog.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "picohttpparser.h"
#include "hilos.h"

#define MAX_HEADER 4096
#define NOMBRE_SERVIDOR "Elpeonkamikaze"
#define VERSION_SERVIDOR "1.0"
#define MAX_ANSWER 100000

typedef struct {

    char *method;
    size_t method_len;
    char *path;
    size_t path_len;
    struct phr_header headers[100];
    size_t num_headers;
    int minor_version;


} Request;


int parsear_conexion(int socketfd, Request* request);
Request* request_create();
int procesar_conexion(int socketfd);
char * construir_cabecera(char *codigo,char *path_recurso);