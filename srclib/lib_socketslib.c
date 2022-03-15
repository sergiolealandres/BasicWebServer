
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
    //printf("HOOOOOOOOOLAAAAAAA\n");
    int desc, len;
    struct sockaddr Conexion;

    len = sizeof(Conexion);
    //printf("pre accept\n");
    
    if ((desc = accept(sockval, &Conexion, &len))<0){
        //printf("el locoooooooo\n");
        
        if(errno==EINTR)return 0;
        
        syslog(LOG_ERR, "Error accepting connection");
        exit(EXIT_FAILURE);
    }
    //printf("va bien accept\n");

    return desc;
}

