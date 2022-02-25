#include <errno.h>
#include <assert.h>
#include "../includes/procesar.h"

Request* request_create(){

    Request* request=(Request*)malloc(sizeof(Request));
    return request;
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
    
    //printf("pos_punto es %d\n",pos_punto);

    if(pos_punto == 5) return -1;

    

    for(i=0;i<pos_punto;i++){
        final[i] = path[len - pos_punto + i -1];
    }

    final[i] = '\0';
    //printf("final es %s\n",final);
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

int parsear_conexion(int socketfd, Request *request){


    char buf[4096], *method, *path;
    int pret, minor_version, i;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
    ssize_t rret;
    printf("dentro del parseoooooooooooooooooooooooo\n");
    while (1) {
        /* read the request */
        while ((rret = read(socketfd, buf + buflen, sizeof(buf) - buflen)) == -1 && errno == EINTR)
            ;
        if (rret <= 0)
            return -1;
        prevbuflen = buflen;
        buflen += rret;
        /* parse the request */
        num_headers = sizeof(headers) / sizeof(headers[0]);
        //pret = phr_parse_request(buf, buflen, &(request->method), &(request->method_len), &(request->path), &(request->path_len),
        //                        &(request->minor_version), request->headers, &(request->num_headers), prevbuflen);
        //if (pret > 0)
        //    break; /* successfully parsed the request */
        //else if (pret == -1)
        //    return -1;
        /* request is incomplete, continue the loop */
       // if (buflen == sizeof(buf))
        //    return -1;
    }
    return 0;
    /*printf("HOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOLA\n");
    printf("request is %d bytes long\n", pret);
    printf("method is %.*s\n", (int)method_len, method);
    printf("path is %.*s\n", (int)path_len, path);
    printf("HTTP version is 1.%d\n", minor_version);
    printf("headers:\n");
    for (i = 0; i != num_headers; ++i) {
        printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name,
            (int)headers[i].value_len, headers[i].value);
    }
    */

}
int procesar_conexion(int socketfd){

    Request *request;
    
    request=request_create();

    if(request==NULL)return -1;

    if(parsear_conexion(socketfd, request)) return -1;


}
char * construir_cabecera(char *codigo,char *path_recurso){
    struct stat attr;
    char *cabecera;
    char date[1000],lastmodified[1000], tipo_fic[1000];
    int size_recurso;
    time_t now;
    struct tm tm; 
    FILE *f;
    size_t len;

    if(!codigo) return NULL;
    cabecera = (char*)malloc(MAX_HEADER);
    if(!cabecera) return NULL;

    now = time(NULL);
    tm = *gmtime(&now);
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    sprintf(cabecera,"HTTP/1.1  %s\r\nDate: %s\r\nServer: %s/%s\r\n",codigo,date,NOMBRE_SERVIDOR,VERSION_SERVIDOR);
    //printf("Primera cabecera:\n%s\n", cabecera);
    if(path_recurso){
        stat(path_recurso,&attr);
        tm = *gmtime(&attr.st_mtime);
        strftime(lastmodified, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);
        if(!(f=fopen(path_recurso,"r"))) return NULL;
        fseek(f, 0L, SEEK_END);
        size_recurso = ftell(f);

        //get the file type
        printf("Antes de tipo_fichero\n");
        if(tipo_fichero(path_recurso,tipo_fic)) return NULL;
        printf("Después de tipo_fichero\n");

        sprintf(cabecera,"%sLast-Modified: %s\r\nContent-Length: %d\r\nContent-Type: %s",cabecera,lastmodified,size_recurso,tipo_fic);
    }
    printf("La cabecera resultante es: \n\n%s\n\n",cabecera);
    return cabecera;
}


int get(Request *r){
    


    return 0;
}

