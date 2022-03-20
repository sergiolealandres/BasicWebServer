/**********************************
 * @author Pablo Soto y Sergio Leal
 * Grupo 2301
 * Redes de Comunicaciones II
 * Práctica 1:Servidor Web
 * Curso 21/22
 * *******************************/
#include "../includes/lib_socketlib.h"


int initiate_server(long port,long max_connections){

    int sockval;
    struct sockaddr_in Direccion;

    syslog (LOG_INFO, "Creating socket");
    if ( (sockval = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        syslog(LOG_ERR, "Error creating␣socket");
        exit(EXIT_FAILURE);
    }

    
    int reuse = 1;
    if (setsockopt(sockval, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        syslog(LOG_ERR, "setsockopt(SO_REUSEADDR) failed");

    Direccion.sin_family=AF_INET; // TCP/IP family 
    Direccion.sin_port=htons(port); // Asigning port
    Direccion.sin_addr.s_addr=htonl(INADDR_ANY); // Accept all adresses
    bzero((void *)&(Direccion.sin_zero), 8);
    
    syslog (LOG_INFO, "Binding socket");
    if (bind (sockval, (struct sockaddr *)&Direccion, sizeof(Direccion))<0){
        syslog(LOG_ERR, "Error binding socket");
        
        exit(EXIT_FAILURE);
    }
    
    syslog (LOG_INFO, "Listening connections");
    if (listen (sockval, max_connections)<0){
        syslog(LOG_ERR, "Error listenining");
        exit(EXIT_FAILURE);
    }

    return sockval;

    }


int accept_connection(int sockval){
    
    int desc;
    unsigned int len;
    struct sockaddr Conexion;

    len = sizeof(Conexion);
    
    while ((desc = accept(sockval, &Conexion, &len))<0){
        
        if(errno==EAGAIN || errno==EWOULDBLOCK ){
            
            fcntl(sockval, F_SETFL, ((const int)(fcntl(sockval, F_GETFL, 0))) ^O_NONBLOCK);
        }
        
        else if(errno ==EINTR)return 0;

    }

    return desc;
}

