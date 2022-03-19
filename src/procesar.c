/*****************************************
 * @author Pablo Soto y Sergio Leal
 * Grupo 2301
 * Redes de Comunicaciones II
 * Práctica 1:Servidor Web
 * Curso 21/22
 * ***************************************/
#include "../includes/procesar.h"

int tipo_fichero(char* path,char *tipo){
    char final[6];
    size_t len;
    int i;
    int pos_punto;

    if(!path) return -1;

    len = strlen(path);
    if(len < 5) return -1;

    for(pos_punto=0;pos_punto<5&&path[len-pos_punto-1]!='.';pos_punto++);
    
    if(pos_punto == 5) return -1;

    for(i=0;i<=pos_punto;i++){
        final[i] = path[len - pos_punto + i -1];
    }

    final[i] = '\0';

    if(!strcmp(final,".txt")) sprintf(tipo,"text/plain");
    else if (!strcmp(final,".html")) sprintf(tipo,"text/html");
    else if (!strcmp(final,".htm")) sprintf(tipo,"text/html");
    else if (!strcmp(final,".gif")) sprintf(tipo,"image/gif");
    else if (!strcmp(final,".jpeg")) sprintf(tipo,"image/jpeg");
    else if (!strcmp(final,".jpg")) sprintf(tipo,"image/jpeg");
    else if (!strcmp(final,".mpeg")) sprintf(tipo,"video/mpeg");
    else if (!strcmp(final,".mpg")) sprintf(tipo,"video/mpeg");
    else if (!strcmp(final,".doc")) sprintf(tipo,"application/msword");
    else if (!strcmp(final,".docx")) sprintf(tipo,"application/msword");
    else if (!strcmp(final,".pdf")) sprintf(tipo,"application/pdf");
    else if (!strcmp(final,".py")) sprintf(tipo,"application/x-python-code");
    else if (!strcmp(final,".php")) sprintf(tipo,"application/x-php");
    else return -1;
    return 0;
}



int procesar_conexion(int socketfd,char *server_root, char * server_signature){

    Request request;
    int parse;

    
    
    request.buflen=0;

    //Parseamos la petición, obteniendo los valores que nos interesan y guardándolos en la
    //estructura request
    parse=parsear_peticion(socketfd, &request);


    if(parse==-2){
        mandar_respuesta(socketfd,"400 Bad Request",NULL,server_signature,-1);
    
        return -1;
    }

    if(parse==-1)return -1;


    if(request.minor_version != 1){
        mandar_respuesta(socketfd,"505 HTTP Version Not Supported",NULL,server_signature,-1);
        
        return 0;
    }

    if (strncmp(request.method, "GET", request.method_len) == 0)
    get(socketfd, request,server_root,server_signature);//PROCESAMOS UN GET

    else if(strncmp(request.method, "POST", request.method_len) == 0)
    post(socketfd, request,server_root,server_signature);//PROCESAMOS UN POST

    else if(strncmp(request.method, "OPTIONS", request.method_len) == 0)
    options(socketfd, request,server_signature);//PROCESAMOS UN OPTIONS

    else if(strncmp(request.method, "HEAD", request.method_len) == 0)
    head(socketfd, request,server_root,server_signature);//PROCESAMOS UN HEAD

    //En caso de no tratarse de ninguna de las anteriores peticiones, se manda not implemented
    else mandar_respuesta(socketfd,"501 Not Implemented",NULL,server_signature,-1);
    
    return 0;
}

void options(int socketfd, Request request, char *server_signature){

    mandar_respuesta(socketfd,"200 OK",NULL,server_signature,1);//MANDA LA CABECERA ÚNICAMENTE

}

void head(int socketfd, Request r, char *server_root, char *server_signature){

    FILE *f;
    char real_path[MAX_PATH];


    if (!(r.path)){
        mandar_respuesta(socketfd,"400 Bad Request",NULL,server_signature,-1);
        return;
    }

    
    if(r.path_len==1){
        sprintf(real_path,"%s/index.html",server_root);//Caso base (cargar el index.html)
    }

    else{

        sprintf(real_path,"%s%.*s",server_root,(int)r.path_len,r.path);//construimos el path real
    }

    if(!(f=fopen(real_path,"r"))){
        mandar_respuesta(socketfd,"404 Not Found",NULL,server_signature,-1);
        return;
    }

    fclose(f);
    mandar_respuesta(socketfd,"200 OK",real_path,server_signature,2);//mandamos la respuesta indicando que se trata de un head

    return;

}


char * construir_cabecera(char *codigo,char *path_recurso,char *server_signature, int flagOptions){
    struct stat attr;
    char cabecera[MAX_HEADER],*cabecera2;
    char recursosfichero[MAX_HEADER];
    char date[ONE_KB],lastmodified[ONE_KB], tipo_fic[ONE_KB];
    int size_recurso;
    time_t now;
    struct tm tm; 
    FILE *f;

    if(!codigo) return NULL;

    cabecera2 = (char*)malloc(2*MAX_HEADER);
    if(!cabecera2) return NULL;

    now = time(NULL);
    tm = *gmtime(&now);
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    if(flagOptions==1){
        //En caso de que se trate de un OPTIONS, tenemos que añadir el ALLOWS a la cabecera
        sprintf(cabecera,"HTTP/1.1  %s\r\nAllow: %s\r\nDate: %s\r\nServer: %s\r\n",codigo,ALLOWS,date,server_signature);
    }
    else{
        //En caso contrario construimos la primera parte de la cabecera de manera normal
        sprintf(cabecera,"HTTP/1.1  %s\r\nDate: %s\r\nServer: %s\r\n",codigo,date,server_signature);
    }
    if(path_recurso){
        stat(path_recurso,&attr);
        tm = *gmtime(&attr.st_mtime);//Obtenemos la hora y fecha
        strftime(lastmodified, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);
        if(!(f=fopen(path_recurso,"r"))) return NULL;//Abrimos sobre un fichero el recurso a cargar
        fseek(f, 0L, SEEK_END);
        size_recurso = ftell(f);
        fclose(f);

        if(tipo_fichero(path_recurso,tipo_fic)) return NULL;


        //Terminamos de constuirm la cabecera
        sprintf(recursosfichero,"Last-Modified: %s\r\nContent-Length: %d\r\nContent-Type: %s\r\n",lastmodified,size_recurso,tipo_fic);
        strcat(cabecera,recursosfichero);
    }
    sprintf(cabecera2,"%s\r\n",cabecera);

    return cabecera2;
}

void mandar_respuesta(int socketfd,char *codigo,char *path,char *server_signature, int flagOption){
    char *cabecera;
    int f;
    char codigoError[SMALL_SIZE];
    long file_length=0;
    long acc_sent = 0;
    ssize_t already_sended_size;

    if(!codigo) return;
    
    //Llamamos a la función de constuir cabecera
    if(!(cabecera=construir_cabecera(codigo,path,server_signature, flagOption)))return;

    //Mandamos al servidor la cabecera por el socket

    send(socketfd,cabecera,strlen(cabecera),0);
    free(cabecera);

    if(flagOption == -1){//En caso de que se trate de un error 404 mandamos un mensaje de error como cuerpo
        sprintf(codigoError,"Error: %s\r\n\r\n",codigo);
        send(socketfd,codigoError,strlen(codigoError),0);
    }
    else if(path && flagOption!=2){
        if(!(f=open(path,O_RDONLY))) return;
        file_length = lseek(f, 0L, SEEK_END);//Nos situamos al final del fichero
        lseek(f,0,0);

        acc_sent=0;
        while(acc_sent<file_length){
            //Hacemos uso de la función sendfile() para mandar al servidor los archivos
            
            already_sended_size =sendfile(socketfd,f,NULL,file_length);
            
            acc_sent += already_sended_size;  
        }

        close(f);
    }
    
    syslog (LOG_INFO, "Message sent");
    
}

void get(int socketfd, Request r,char * server_root,char * server_signature){
    FILE *f;
    char real_path[MAX_PATH], aux_path[MAX_PATH], *type_data=NULL;
    char *dirtyPath, *cleanPath, comando[MAX_HEADER];
    
    //En caso de tratarse de un path vacío o una petición inexistente se devuelve bad request
    if (!(r.path)){
        mandar_respuesta(socketfd,"400 Bad Request",NULL,server_signature,-1);
        return;
    }
    
    //Caso base, cargar el index.html
    if(r.path_len==1){

        sprintf(real_path,"%s/index.html",server_root);
    }
    
    else{
        
        sprintf(real_path,"%s%.*s",server_root,(int)r.path_len,r.path);
        sprintf(aux_path,"%.*s",(int)r.path_len,r.path);

        //Obtenemos la terminación del fichero para comprobar si es un script
        type_data = strtok(aux_path, ".");
        type_data = strtok(NULL, "?");

        if(type_data==NULL){
        mandar_respuesta(socketfd,"404 Not Found",NULL,server_signature,-1);
        return;
        }

        //En caso de que se trate de un script
        if(strncmp(type_data, "py", strlen(real_path))==0   || strncmp(type_data, "php", strlen(real_path))==0){

            dirtyPath=strtok(real_path, ".");
            
            cleanPath=strtok(NULL, ".");
            
            
            //Quitamos todos los '+' y construimos el path 'limpio'
            char *aux3, realbuff[100000]="\0";

            strtok(cleanPath, "?");
            aux3=strtok(NULL, "?");

            //Limpiamos el path de '+' y construimos uno nuevo
            dirtyPath=strtok(aux3, "+");
            while((dirtyPath)!=NULL){            

                strcat(realbuff, dirtyPath);
                strcat(realbuff, " ");
                dirtyPath=strtok(NULL, "+");        
            } 
            strtok(realbuff, "=");
            dirtyPath=strtok(NULL, "=");

            
            if(strncmp(type_data, "py", strlen(real_path))==0)
                //Si es un .py añadimos python3 al comando
                sprintf(comando, "python3 %s.py %s ", real_path, dirtyPath);

            else
                //Si es un .php añadimos php al comando
                sprintf(comando, "php %s.php %s ", real_path, dirtyPath);

            //Ejecutamos el comando e imprimimos la salida por pantalla
            
            executeAndPrintOnScreen(socketfd, comando,server_signature);

            return;
            
        }
    }
    if(!(f=fopen(real_path,"r"))){
        mandar_respuesta(socketfd,"404 Not Found",NULL,server_signature,-1);
        return;
    }

    fclose(f);
    //Si todo ha ido bien mandamos la respuesta con código 200 OK
    mandar_respuesta(socketfd,"200 OK",real_path,server_signature,0);

    return;
}

void post(int socketfd, Request r, char* server_root, char * server_signature){

    char* type_data=NULL, aux_path[MAX_PATH]="\0", real_path[MAX_PATH];
    char *path_for_command, comando[MAX_ANSWER], buffer[SMALL_SIZE]="\0";
    char *aux_string=NULL, aux_buffer[MAX_PATH]="\0";
    int flag=0,i, size_recurso=-1;
    
    

    if (!(r.path)){
        mandar_respuesta(socketfd,"400 Bad Request",NULL,server_signature,-1);
        return;
    }

    
    sprintf(real_path,"%s%.*s",server_root,(int)r.path_len,r.path);

    path_for_command=strtok(real_path, "?");

    sprintf(aux_path,"%.*s",(int)r.path_len,r.path);
    
    type_data = strtok(aux_path, ".");

    type_data = strtok(NULL, "?");

    if(type_data==NULL){
        mandar_respuesta(socketfd,"404 Not Found",NULL,server_signature,-1);
        return;
    }

    //En caso de tratarse de un script
    if(strncmp(type_data, "py", strlen(real_path))==0   || strncmp(type_data, "php", strlen(real_path))==0){
        
        flag=searchForContentLength(r, &i);
        if(flag==1){//Buscamos si hay cuerpo buscando la cabecera Content-Length
           
            size_recurso=atoi(r.headers[i].value);//Obtenemos la longitud del contenido
            
            strncpy(buffer, r.buf + r.buflen - size_recurso, (size_t)size_recurso);//obtenemos el content
           
            aux_string=strtok(buffer, "+");
            
            
            //"Limpiamos" el cuerpo eliminando los '+' y construyéndolo de nuevo
            while(aux_string!=NULL){            

                strcat(aux_buffer, aux_string);
                strcat(aux_buffer, " ");
                aux_string=strtok(NULL, "+");
    
            }
            
            strtok(aux_buffer, "=");
            aux_string=strtok(NULL, "=");

        }

        if(strncmp(type_data, "py", strlen(real_path))==0){//En caso de ser un .py

            if(flag==1) sprintf(comando, "python3 %s %s ", path_for_command, aux_string);
                
            else sprintf(comando, "python3 %s ", path_for_command);            
        }
        else{//En caso de ser un .php
            if(flag==1) sprintf(comando, "php %s %s ", path_for_command, aux_string);
                
            else sprintf(comando, "php %s ", path_for_command);            
        }
        //LLamamos a la función que ejecuta el comando e manda la salida al server
        executeAndPrintOnScreen(socketfd, comando,server_signature);
        return;
    }
    else{

        //En caso de no ser un script se devuelve not implemented 
        mandar_respuesta(socketfd,"404 Not Found",NULL,server_signature,-1);
    }
}

int executeAndPrintOnScreen(int socketfd, char*comando, char* server_signature){
    char cabecera[ONE_KB];
    FILE *file;
    char buffer2[MAX_PATH]="\0", line[MAX_PATH];
    time_t now;
    struct tm tm;
    int acc = 0;
    char date[SMALL_SIZE];
    char *auxiliar_string;

    file=popen(comando, "r");//Ejecutamos el comando obteniendo la salida sobre un fichero
    if(!file)return -1;
    
    //Construimos, sobre un buffer, una cadena linea a linea con la salida que teníamos en el file
    while ((auxiliar_string = fgets(line, ONE_KB, file)) != NULL)
    {
        strcat(buffer2, line);
    }
    acc = strlen(buffer2);
    
    //fecha y hora actual
    now = time(NULL);
    tm = *gmtime(&now);
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    sprintf(cabecera,"HTTP/1.1 200 OK\r\nDate: %s\r\nServer: %s\r\nContent-Length: %d\r\nContent-Type: text/plain\r\n\r\n",date,server_signature,acc);
    //Mandamos la cabecera
    send(socketfd,cabecera,strlen(cabecera),0);

    //Mandamos el cuerpo
    send(socketfd, buffer2, strlen(buffer2), 0);

    pclose(file);//cerramos la pipe
    return 0;
}

int searchForContentLength(Request r, int *position){
    int i;

    //Recorremos las cabeceras buscando la de Content-Lenght
    for(i=0;i<r.num_headers;i++){

        if(strncmp(r.headers[i].name, "Content-Length:", 15)==0){
            *position=i;
            return 1;
        }
    }
    return 0;
}


int parsear_peticion(int socketfd, Request *request){

    size_t prevbuflen = 0;
    ssize_t rret;
    int pret;


    while (1) {

        struct timeval timeout;      
        timeout.tv_sec = 8;
        timeout.tv_usec = 0;
        
        //Establecemos un timeout para el caso de ue alguien abra una conexión pero no envíe nada.
        //Así conseguimos que un hilo no se quede inutilizable
        if (setsockopt (socketfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                    sizeof timeout) < 0){
                        perror("setsockopt failed\n");
                    }
        
        //Utilizamos la función read() para leer del socket, la petición puede ser de tamaño muy largo por lo que utilizamos un while
        while ((rret = read(socketfd, (request)->buf + (request)->buflen, sizeof((request)->buf) - (request)->buflen)) == -1){
            
            if(errno == EWOULDBLOCK || errno==EINTR){
                
                return -1;
            }
            
        }
        
        

        if (rret <= 0){
           
            return -1;
        }
            
            
        prevbuflen = (request)->buflen;
        (request)->buflen += rret;

        (request)->num_headers = sizeof((request)->headers) / sizeof((request)->headers[0]);

        //Utilizamos la función de la librería auxiliar para el parseo
        pret = phr_parse_request((request)->buf, (request)->buflen, (const char**) &((request)->method), 
                                &((request)->method_len),(const char**) &((request)->path), &((request)->path_len),
                                &((request)->minor_version), (request)->headers, &((request)->num_headers), prevbuflen);

       
        if (pret > 0)
            break; 
        else if (pret == -1){
            
            return -2;
        }
        if ((request)->buflen == sizeof((request)->buf)){
            
            return -1;
        }
        
    }
    return 0;

}

