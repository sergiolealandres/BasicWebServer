#include "../includes/lib_socketlib.h"


int main(int argc, char **argv){

    int listenfd, connfd;
    socklen_t clilen, addrlen;
    struct sockaddr *cliaddr;

    // Contiene las llamadas a socket(), bind() y listen()
    listenfd = initiate_server();

    while (1){
        accept_connection(listenfd);
    }
    return 0;
}