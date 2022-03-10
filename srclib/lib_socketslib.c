
#include "../includes/lib_socketlib.h"


int initiate_server(void){

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
    Direccion.sin_port=htons(NFC_SERVER_PORT); // Asigning port
    Direccion.sin_addr.s_addr=htonl(INADDR_ANY); // Accept all adresses
    bzero((void *)&(Direccion.sin_zero), 8);

    syslog (LOG_INFO, "Binding socket");
    if (bind (sockval, (struct sockaddr *)&Direccion, sizeof(Direccion))<0){
        syslog(LOG_ERR, "Error binding socket");
        exit(EXIT_FAILURE);
    }

    syslog (LOG_INFO, "Listening connections");
    if (listen (sockval, MAX_CONNECTIONS)<0){
        syslog(LOG_ERR, "Error listenining");
        exit(EXIT_FAILURE);
    }

    return sockval;

    }

void launch_service(int connval){
    char *mensaje = "HTTP/1.1 200 OK\r\n\r\nHola Buenas Tardes\n";
    int pid;
    long type, aux;

    syslog (LOG_INFO, "New access");
    send(connval,mensaje, strlen(mensaje),0);
    syslog (LOG_INFO, "Message sent");

    //recv(connval, &aux, sizeof(long), 0);
    //type = ntohl(aux);
    //syslog (LOG_INFO, "Message received");
    //database_access(connval, type, NULL);
    close(connval);
    syslog (LOG_INFO, "Exiting service");
    return;
}

int accepto_connection(int sockval){
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

