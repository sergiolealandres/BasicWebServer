
#include "../includes/procesar.h"

Request* request_create(){

    Request* request=(Request*)malloc(sizeof(Request));

    return request;
}

void request_free(Request *request){
    
    if(request){
        free(request);
    }
}

Request* request_copy(Request *r){
    int i;
    Request *cpy = request_create();

    if(!cpy || !r) return NULL;

    cpy->method = (char*)malloc(r->method_len);
    strncpy(cpy->method,r->method,r->method_len);
    cpy->method_len=r->method_len;

    cpy->path = (char*)malloc(r->path_len);
    strncpy(cpy->path,r->path,r->path_len);
    cpy->path_len=r->path_len;

    cpy->minor_version = r->minor_version;

    for(i=0;i<r->num_headers;i++){
        cpy->headers[i]=r->headers[i];
    }
    cpy->num_headers = r->num_headers;
    return cpy;
}

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
    else return -1;
    return 0;
}

int procesar_conexion(int socketfd,char *server_root, char * server_signature){

    Request *request;
    int parse;

    if(!(request=request_create()))return -1;
    
    request->buflen=0;

    parse=parsear_peticion(socketfd, &request);

    if(parse==-1)return -1;

    if(request->minor_version != 1){
        mandar_respuesta(socketfd,"505 HTTP Version Not Supported",NULL,server_signature,0);
        request_free(request);
        return 0;
    }

    if (strncmp(request->method, "GET", request->method_len) == 0)get(socketfd, request,server_root,server_signature);

    else if(strncmp(request->method, "POST", request->method_len) == 0)post(socketfd, request,server_root,server_signature);

    else if(strncmp(request->method, "OPTIONS", request->method_len) == 0)options(socketfd, request,server_signature);

    else if(strncmp(request->method, "HEAD", request->method_len) == 0)head(socketfd, request,server_root,server_signature);

    else mandar_respuesta(socketfd,"501 Not Implemented",NULL,server_signature,0);

    request_free(request);
    
    return 0;
}

void options(int socketfd, Request *request, char *server_signature){

    mandar_respuesta(socketfd,"200 OK",NULL,server_signature,1);

}

void head(int socketfd, Request *r, char *server_root, char *server_signature){

    FILE *f;
    char real_path[MAX_PATH];


    if (!r|| !(r->path)){
        mandar_respuesta(socketfd,"400 Bad Request",NULL,server_signature,0);
        return;
    }

    
    if(r->path_len==1){
        sprintf(real_path,"%s/index.html",server_root);
    }

    else{

        sprintf(real_path,"%s%.*s",server_root,(int)r->path_len,r->path);
    }

    if(!(f=fopen(real_path,"r"))){
        mandar_respuesta(socketfd,"404 Not Found",NULL,server_signature,0);
        return;
    }

    fclose(f);
    mandar_respuesta(socketfd,"200 OK",real_path,server_signature,2);

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
        sprintf(cabecera,"HTTP/1.1  %s\r\nAllow: %s\r\nDate: %s\r\nServer: %s\r\n",codigo,ALLOWS,date,server_signature);
    }
    else{
        sprintf(cabecera,"HTTP/1.1  %s\r\nDate: %s\r\nServer: %s\r\n",codigo,date,server_signature);
    }
    if(path_recurso){
        stat(path_recurso,&attr);
        tm = *gmtime(&attr.st_mtime);
        strftime(lastmodified, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);
        if(!(f=fopen(path_recurso,"r"))) return NULL;
        fseek(f, 0L, SEEK_END);
        size_recurso = ftell(f);
        fclose(f);

        if(tipo_fichero(path_recurso,tipo_fic)) return NULL;

        sprintf(recursosfichero,"Last-Modified: %s\r\nContent-Length: %d\r\nContent-Type: %s\r\n",lastmodified,size_recurso,tipo_fic);
        strcat(cabecera,recursosfichero);
    }
    sprintf(cabecera2,"%s\r\n",cabecera);

    return cabecera2;
}

void mandar_respuesta(int socketfd,char *codigo,char *path,char *server_signature, int flagOption){
    char *cabecera;
    int f;
    long file_length=0;
    long acc_sent = 0;
    ssize_t aux;

    if(!codigo) return;
    
    if(!(cabecera=construir_cabecera(codigo,path,server_signature, flagOption)))return;

    send(socketfd,cabecera,strlen(cabecera),0);

    if(path && flagOption!=2){
        if(!(f=open(path,O_RDONLY))) return;
        file_length = lseek(f, 0L, SEEK_END);
        lseek(f,0,0);

        acc_sent=0;
        while(acc_sent<file_length){

            aux =sendfile(socketfd,f,NULL,file_length);
            if(errno==EINTR)return;

            acc_sent += aux;  
        }

        close(f);
    }
    
    syslog (LOG_INFO, "Message sent");
    free(cabecera);
}

void get(int socketfd, Request *r,char * server_root,char * server_signature){
    FILE *f;
    char real_path[MAX_PATH], aux[MAX_PATH], *type_data=NULL;
    char *aux1, *aux2, comando[MAX_HEADER];
    int i;


    if (!r|| !(r->path)){
        mandar_respuesta(socketfd,"400 Bad Request",NULL,server_signature,0);
        return;
    }
    
    if(r->path_len==1){

        sprintf(real_path,"%s/index.html",server_root);
    }
    
    else{
       
        sprintf(real_path,"%s%.*s",server_root,(int)r->path_len,r->path);
        sprintf(aux,"%.*s",(int)r->path_len,r->path);

        type_data = strtok(aux, ".");

        type_data = strtok(NULL, "?");
        
        if(strncmp(type_data, "py", strlen(real_path))==0   || strncmp(type_data, "php", strlen(real_path))==0){

            aux1=strtok(real_path, ".");
            aux2=strtok(NULL, ".");
            
            if(strncmp(aux2, "py", strlen(type_data))==0   || strncmp(aux2, "php", strlen(type_data))==0){
                
                clean_path_get(&aux2, aux1);

                
                if(strncmp(type_data, "py", strlen(real_path))==0)

                    sprintf(comando, "python3 %s.py %s ", real_path, aux1);

                else

                    sprintf(comando, "php %s.php %s ", real_path, aux1);

                executeAndPrintOnScreen(socketfd, comando,server_signature);

                return;
            }
        }
    }
    if(!(f=fopen(real_path,"r"))){
        mandar_respuesta(socketfd,"404 Not Found",NULL,server_signature,0);
        return;
    }

    fclose(f);
    mandar_respuesta(socketfd,"200 OK",real_path,server_signature,0);

    return;
}


void post(int socketfd, Request *r, char* server_root, char * server_signature){

    char* type_data=NULL, aux[MAX_PATH]="\0", real_path[MAX_PATH];
    char *realreal_path, comando[MAX_ANSWER], buffer[SMALL_SIZE]="\0";
    char *aux1=NULL, realbuff[MAX_PATH]="\0";
    int i, flag=0, size_recurso=-1;
    FILE *file;
    

    if (!r|| !(r->path)){
        mandar_respuesta(socketfd,"400 Bad Request",NULL,server_signature,0);
        return;
    }

    //////printf("POST DETECTADO\n");
    sprintf(real_path,".%.*s",(int)r->path_len,r->path);

    realreal_path=strtok(real_path, "?");

    sprintf(aux,"%.*s",(int)r->path_len,r->path);
    
    type_data = strtok(aux, ".");

    type_data = strtok(NULL, "?");

    
    if(strncmp(type_data, "py", strlen(real_path))==0   || strncmp(type_data, "php", strlen(real_path))==0){
        

        if(searchForContentLenght(r, &i)==1){
           
            size_recurso=atoi(r->headers[i].value);
            
            strncpy(buffer, r->buf + r->buflen - size_recurso, (size_t)size_recurso);
           
            aux1=strtok(buffer, "+");
            
            while(aux1!=NULL){            

                strcat(realbuff, aux1);
                strcat(realbuff, " ");
                aux1=strtok(NULL, "+");
    
            }
            
            strtok(realbuff, "=");
            aux1=strtok(NULL, "=");

        }

        if(strncmp(type_data, "py", strlen(real_path))==0){

            if(flag==1) sprintf(comando, "python3 %s %s ", realreal_path, aux1);
                
            else sprintf(comando, "python3 %s ", realreal_path);            
        }
        else{
            if(flag==1) sprintf(comando, "php %s %s ", realreal_path, aux1);
                
            else sprintf(comando, "php %s ", realreal_path);            
        }
        executeAndPrintOnScreen(socketfd, comando,server_signature);
        return;
    }
}

int executeAndPrintOnScreen(int socketfd, char*comando, char* server_signature){
    char cabecera[ONE_KB];
    FILE *file;
    char buffer2[MAX_PATH]="\0", line[MAX_PATH];
    time_t now;
    struct tm tm;
    int acc = 0,aux;
    char date[SMALL_SIZE];

    file=popen(comando, "r");
    if(!file)return -1;
    
    while (aux = fgets(line, 1000, file) != NULL)
    {
        strcat(buffer2, line);
    }
    acc = strlen(buffer2);
    
    now = time(NULL);
    tm = *gmtime(&now);
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    sprintf(cabecera,"HTTP/1.1 200 OK\r\nDate: %s\r\nServer: %s\r\nContent-Length: %d\r\nContent-Type: text/plain\r\n\r\n",date,server_signature,acc);
    send(socketfd,cabecera,strlen(cabecera),0);
    send(socketfd, buffer2, strlen(buffer2), 0);

    pclose(file);
    return 0;
}

int searchForContentLenght(Request *r, int *position){
    int i;

    for(i=0;i<r->num_headers;i++){

        if(strncmp(r->headers[i].name, "Content-Length:", 15)==0){
            *position=i;
            return 1;
        }
    }
    return 0;
}


int parsear_peticion(int socketfd, Request **request){

    size_t prevbuflen = 0;
    ssize_t rret;
    int pret;

    while (1) {

        struct timeval timeout;      
        timeout.tv_sec = 8;
        timeout.tv_usec = 0;
        
        if (setsockopt (socketfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                    sizeof timeout) < 0){
                        perror("setsockopt failed\n");
                    }

        while ((rret = read(socketfd, (*request)->buf + (*request)->buflen, sizeof((*request)->buf) - (*request)->buflen)) == -1){

            request_free(*request);
            if(errno == EWOULDBLOCK || errno==EINTR){

                return -1;
            }
        }

        if (rret <= 0){
            request_free(*request);
            return -1;
        }
            
            
        prevbuflen = (*request)->buflen;
        (*request)->buflen += rret;

        (*request)->num_headers = sizeof((*request)->headers) / sizeof((*request)->headers[0]);

        pret = phr_parse_request((*request)->buf, (*request)->buflen, (const char**) &((*request)->method), &((*request)->method_len),(const char**) &((*request)->path), &((*request)->path_len),
                                &((*request)->minor_version), (*request)->headers, &((*request)->num_headers), prevbuflen);

        if (pret > 0)
            break; 
        else if (pret == -1){
            request_free((*request));
            return -1;
        }
        if ((*request)->buflen == sizeof((*request)->buf)){
            request_free((*request));
            return -1;
        }
    }

    return 0;

}

void clean_path_get(char **dirtypath, char *cleanpath){

    char *aux3, realbuff[100000]="\0";

    strtok(*dirtypath, "?");
    aux3=strtok(NULL, "?");

    cleanpath=strtok(aux3, "+");
    while((cleanpath)!=NULL){            

        strcat(realbuff, cleanpath);
        strcat(realbuff, " ");
        cleanpath=strtok(NULL, "+");        
    } 
    strtok(realbuff, "=");
    cleanpath=strtok(NULL, "=");  

}

