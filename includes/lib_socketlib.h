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

/**
 * @brief Inicializa el servidor. Llama a las funciones socket(),bind() y listen()
 *  
 * @param port puerto en el que el servidor escucha peticiones
 * @param max_connections maximas conexiones soportadas por el servidor
 * @return socket del servidor
 */
int initiate_server(long port, long max_connections);

/**
 * @brief Se bloquea hasta que se reciba un connect(). 
 * Acepta la conexión y devuelve el socket correspondiente
 * 
 * @param sockval socket del servidor
 * @return socket de la conexión
 */
int accept_connection(int sockval);


#endif
