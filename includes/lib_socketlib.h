/**********************************
 * @author Pablo Soto y Sergio Leal
 * Grupo 2301
 * Redes de Comunicaciones II
 * Práctica 1:Servidor Web
 * Curso 21/22
 * *******************************/

#ifndef SOCKETLIB
#define SOCKETLIB

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define NFC_SERVER_PORT 3000
#define MAX_CONNECTIONS 10

int initiate_server(long port, long max_connections);
int accept_connection(int sockval);
void launch_service(int connval);


#endif
