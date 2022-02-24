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

#define NFC_SERVER_PORT 3000
#define MAX_CONNECTIONS 10

int initiate_server(void);
int accept_connection(int sockval);
void launch_service(int connval);


#endif