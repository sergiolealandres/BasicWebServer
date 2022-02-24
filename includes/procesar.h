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
#include "picohttpparser.h"
#include "hilos.h"

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