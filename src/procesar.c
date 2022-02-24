#include <errno.h>
#include <assert.h>
#include "../includes/procesar.h"

Request* request_create(){

    Request* request=(Request*)malloc(sizeof(Request));
    if(request==NULL)return NULL;
    return request;
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
        pret = phr_parse_request(buf, buflen, &(request->method), &(request->method_len), &(request->path), &(request->path_len),
                                &(request->minor_version), request->headers, &(request->num_headers), prevbuflen);
        if (pret > 0)
            break; /* successfully parsed the request */
        else if (pret == -1)
            return -1;
        /* request is incomplete, continue the loop */
        assert(pret == -2);
        if (buflen == sizeof(buf))
            return -1;
    }
    printf("HOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOLA\n");
    printf("request is %d bytes long\n", pret);
    printf("method is %.*s\n", (int)method_len, method);
    printf("path is %.*s\n", (int)path_len, path);
    printf("HTTP version is 1.%d\n", minor_version);
    printf("headers:\n");
    for (i = 0; i != num_headers; ++i) {
        printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name,
            (int)headers[i].value_len, headers[i].value);
    }


}
int procesar_conexion(int socketfd){

    Request *request;

    request=request_create();

    if(request==NULL)return -1;

    parsear_conexion(socketfd, request);

}

