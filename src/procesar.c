
#include "../includes/procesar.h"

Request* request_create(){

    Request* request=(Request*)malloc(sizeof(Request));

    

    return request;
}

void request_free(Request *request){
    
    if(request){

        //if(request->path)
            //free(request->path);

        //if(request->method)
            //free(request->method);
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
    printf("el path en tipofichero es %s\n",path);
    len = strlen(path);
    if(len < 5) return -1;
    printf("La última letra es %c\n",path[len-1]);

    for(pos_punto=0;pos_punto<5&&path[len-pos_punto-1]!='.';pos_punto++);
    
    //printf("pos_punto es %d\n",pos_punto);

    if(pos_punto == 5) return -1;

    

    for(i=0;i<=pos_punto;i++){
        final[i] = path[len - pos_punto + i -1];
    }

    final[i] = '\0';
    printf("final es %s\n",final);
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

    char prueba[2048];
    char buf[4096];
    int pret, i;
    size_t prevbuflen = 0;
    ssize_t rret;
    Request *request;

    if(!(request=request_create()))return -1;

    
    request->buflen=0;

    while (1) {
        /* read the request */
        while ((rret = read(socketfd, request->buf + request->buflen, sizeof(request->buf) - request->buflen)) == -1 && errno == EINTR)
            ;
        if (rret <= 0)
            return -1;
        prevbuflen = request->buflen;
        request->buflen += rret;
        /* parse the request */
        request->num_headers = sizeof(request->headers) / sizeof(request->headers[0]);
        printf("Antes del phr\n");
        pret = phr_parse_request(request->buf, request->buflen, (const char**) &(request->method), &(request->method_len),(const char**) &(request->path), &(request->path_len),
                                &(request->minor_version), request->headers, &(request->num_headers), prevbuflen);
        printf("pret es %d\n",pret);
        if (pret > 0)
            break; /* successfully parsed the request */
        else if (pret == -1)
            return -1;
        /* request is incomplete, continue the loop */
        if (request->buflen == sizeof(request->buf))
            return -1;
    }
/*
    strncpy(prueba,request->path,request->path_len);
    printf("HOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOLA\n");
    printf("request is %d bytes long\n", pret);
    printf("method is %.*s\n", (int)request->method_len, request->method);
    printf("path is %.*s\n", (int)request->path_len, request->path);
    printf("HTTP version is 1.%d\n", request->minor_version);
    printf("headers:\n");
    for (i = 0; i != request->num_headers; ++i) {
        printf("%.*s: %.*s\n", (int)request->headers[i].name_len, request->headers[i].name,
            (int)request->headers[i].value_len, request->headers[i].value);
    }

    printf("Caracteres: ");
    for(i=0;i<(int)request->path_len;i++){
        printf("%c ",request->path[i]);
    }
    printf("\n");
*/
    if(request->minor_version != 1) mandar_respuesta(socketfd,"505 HTTP Version Not Supported",NULL,server_signature,0);
    
    if (strncmp(request->method, "GET", request->method_len) == 0)get(socketfd, request,server_root,server_signature);


    else if(strncmp(request->method, "POST", request->method_len) == 0)post(socketfd, request,server_root,server_signature);

    else if(strncmp(request->method, "OPTIONS", request->method_len) == 0)options(socketfd, request,server_signature);

    else if(strncmp(request->method, "HEAD", request->method_len) == 0)head(socketfd, request,server_root,server_signature);

    else mandar_respuesta(socketfd,"501 Not Implemented",NULL,server_signature,0);

    request_free(request);
    
    return 0;
}

void options(int socketfd, Request *request, char *server_signature){


    printf("OPTIONS RECIBIDO\n");
    mandar_respuesta(socketfd,"200 OK",NULL,server_signature,1);

}

void head(int socketfd, Request *r, char *server_root, char *server_signature){


    FILE *f;
    char real_path[MAX_PATH], real_path2[MAX_PATH], aux[MAX_PATH], *type_data=NULL, *aux1, *aux2, *aux3, *aux4, comando[2*MAX_SENT];
    int i, script=0;
    char realbuff[MAX_PATH]="\0";
    FILE *file;
    char line[1000], buffer2[1000];



    //printf("En get\n");
    if (!r|| !(r->path)){
        mandar_respuesta(socketfd,"400 Bad Request",NULL,server_signature,2);
        return;
    }

    //printf("pathlen es: %ld\n", r->path_len);
    
    if(r->path_len==1){
        printf("eo1\n");
        sprintf(real_path,"%s/index.html",server_root);
    }

    else{

        sprintf(real_path,"%s%.*s",server_root,(int)r->path_len,r->path);
    }

    if(!(f=fopen(real_path,"r"))){
        mandar_respuesta(socketfd,"404 Not Found",NULL,server_signature,2);
        return;
    }

    fclose(f);
    mandar_respuesta(socketfd,"200 OK",real_path,server_signature,2);

    return;

}


char * construir_cabecera(char *codigo,char *path_recurso,char *server_signature, int flagOptions){
    struct stat attr;
    char *cabecera;
    char recursosfichero[3000];
    char date[1000],lastmodified[1000], tipo_fic[1000];
    int size_recurso;
    time_t now;
    struct tm tm; 
    FILE *f;
    size_t len;

    if(!codigo) return NULL;
    cabecera = (char*)malloc(2*MAX_HEADER);
    if(!cabecera) return NULL;

    now = time(NULL);
    tm = *gmtime(&now);
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    printf("PACO\n");
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
        //get the file type
        printf("Antes de tipo_fichero\n");
        if(tipo_fichero(path_recurso,tipo_fic)) return NULL;
        printf("Después de tipo_fichero\n");
        printf("Las modified es:%s, \n content-length es %d\n content type es %s\n",lastmodified,size_recurso,tipo_fic);
        printf("PACO\n");
        sprintf(recursosfichero,"Last-Modified: %s\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n",lastmodified,size_recurso,tipo_fic);
        strcat(cabecera,recursosfichero);
    }
    
    printf("La cabecera resultante es: \n\n%s\n\n",cabecera);
    return cabecera;
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
            printf("aux es %ld\n",aux);
            acc_sent += aux;
            printf("acc_sent es %ld y file_length es %ld\n",acc_sent,file_length);
        }

        close(f);
    }
    
    syslog (LOG_INFO, "Message sent");
    free(cabecera);
}

void get(int socketfd, Request *r,char * server_root,char * server_signature){
    FILE *f;
    char real_path[MAX_PATH], real_path2[MAX_PATH], aux[MAX_PATH], *type_data=NULL, *aux1, *aux2, *aux3, *aux4, comando[2*MAX_SENT];
    int i, script=0;
    char realbuff[MAX_PATH]="\0";
    FILE *file;
    char line[1000], buffer2[1000];



    //printf("En get\n");
    if (!r|| !(r->path)){
        mandar_respuesta(socketfd,"400 Bad Request",NULL,server_signature,0);
        return;
    }

    //printf("pathlen es: %ld\n", r->path_len);
    
    if(r->path_len==1){
        printf("eo1\n");
        sprintf(real_path,"%s/index.html",server_root);
    }

    else{
        printf("eo2\n");
    sprintf(real_path,"%s%.*s",server_root,(int)r->path_len,r->path);
    sprintf(aux,"%.*s",(int)r->path_len,r->path);
//    strncpy(aux, real_path, strlen(real_path));
    
    //strncpy(aux, real_path, strlen(real_path));
    printf("REAL PATHHHH%s\n", real_path);
    printf("eo2\n");
    type_data = strtok(aux, ".");
    printf("eo2\n");
    type_data = strtok(NULL, "?");
    printf("eo2\n");
    
    if(strncmp(type_data, "py", strlen(real_path))==0   || strncmp(type_data, "php", strlen(real_path))==0){

        aux1=strtok(real_path, ".");
        aux2=strtok(NULL, ".");

        //printf("AUX1 Y AUX2: %s     %s\n", aux1, aux2);

        if(strncmp(aux2, "py", strlen(type_data))==0   || strncmp(aux2, "php", strlen(type_data))==0){
            
            strtok(aux2, "?");
            aux3=strtok(NULL, "?");

            aux1=strtok(aux3, "+");
            while(aux1!=NULL){            

                strcat(realbuff, aux1);
                strcat(realbuff, " ");
                aux1=strtok(NULL, "+");
                
            }

            strtok(realbuff, "=");
            aux1=strtok(NULL, "=");

            

            if(strncmp(type_data, "py", strlen(real_path))==0)

                sprintf(comando, "python3 %s.py %s ", real_path, aux1);

            else

                sprintf(comando, "php %s.php %s ", real_path, aux1);

            executeAndPrintOnScreen(socketfd, comando,server_signature);
            return;


        }


    }
    
    printf("eo\n");
    }

    printf("EL path de r es %.*s\n",(int)r->path_len, r->path);
    printf("EL path es %s\n",real_path);

    if(!(f=fopen(real_path,"r"))){
        mandar_respuesta(socketfd,"404 Not Found",NULL,server_signature,0);
        return;
    }

    fclose(f);
    mandar_respuesta(socketfd,"200 OK",real_path,server_signature,0);

    return;
}


void post(int socketfd, Request *r, char* server_root, char * server_signature){

    char* type_data=NULL, aux[2000]="\0", real_path[2000],*realreal_path, comando[100000], buffer[100]="\0", *aux1=NULL, realbuff[100]="\0";
    int i, flag=0, size_recurso=-1;
    FILE *file;
    char line[1000] = "\0";
    char buffer2[1000];

    printf("POST DETECTADO\n");
    sprintf(real_path,".%.*s",(int)r->path_len,r->path);

    realreal_path=strtok(real_path, "?");


    printf("eo\n");
    sprintf(aux,"%.*s",(int)r->path_len,r->path);
    //strncpy(aux, real_path, strlen(real_path));
    printf("AUX ES %s\n", realreal_path);
    printf("eo\n");
    type_data = strtok(aux, ".");
    printf("eo\n");
    type_data = strtok(NULL, "?");
    printf("eo\n");

    if(strncmp(type_data, "py", strlen(real_path))==0   || strncmp(type_data, "php", strlen(real_path))==0){

        for(i=0;i<r->num_headers&&flag==0;i++){

            if(strncmp(r->headers[i].name, "Content-Length:", 15)==0){
                printf("flag=1\n");
                flag=1;
            }
        }
        

        if(flag==1){

            size_recurso=atoi(r->headers[i-1].value);
            
            strncpy(buffer, r->buf + r->buflen - size_recurso, (size_t)size_recurso);
            aux1=strtok(buffer, "+");
            while(aux1!=NULL){            

                strcat(realbuff, aux1);
                printf("eo3\n");
                strcat(realbuff, " ");
                printf("eo1\n");
                aux1=strtok(NULL, "+");
                printf("eo2\n");
                
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
    char *cabecera;
    FILE *file;
    char buffer2[MAX_PATH]="\0", line[MAX_PATH];

    file=popen(comando, "r");
    if(!file)return -1;
    
    while (fgets(line, 1000, file) != NULL)
    {
        strcat(buffer2, line);
    }
    
    if(!(cabecera=construir_cabecera("200 OK",NULL,server_signature, 0)))return -1;

    send(socketfd,cabecera,strlen(cabecera),0);


    send(socketfd, buffer2, strlen(buffer2), 0);

    pclose(file);
    free(cabecera);
    return 0;
}

