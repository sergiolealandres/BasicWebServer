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
#define ALLOWS "OPTIONS, GET, POST"
#define MAX_ANSWER 100000
#define MAX_PATH 2048
#define MAX_SENT 2048

typedef struct {

    char *method;
    size_t method_len;
    char *path;
    size_t path_len;
    struct phr_header headers[100];
    size_t num_headers;
    int minor_version;
    char buf[MAX_HEADER];
    size_t buflen;


} Request;

Request* request_copy(Request *r);
int parsear_conexion(int socketfd, Request** request);
Request* request_create();
void request_free(Request *request);
int procesar_conexion(int socketfd,char *server_root, char *server_signature);
char * construir_cabecera(char *codigo,char *path_recurso,char *server_signature, int flagOptions);
void get(int socketfd, Request *r,char *server_root,char *server_signature);
void post(int socketfd, Request *r,char *server_root,char *server_signature);
int executeAndPrintOnScreen(int socketfd, char*comando,char* server_signature);
void mandar_respuesta(int socketfd,char *codigo,char *path,char *server_signature, int flagOptions);
void options(int socketfd, Request *request, char *server_signature);
void head(int socketfd, Request *r, char *server_root, char *server_signature);
#endif
