#include "../includes/lib_socketlib.h"
#include "../includes/procesar.h"
#include "../includes/hilos.h"

#include <confuse.h>

#define MAX_HILOS 10


pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
static volatile int got_signal=0;

void do_daemon(void){
    pid_t pid;

    pid = fork(); /* Fork off the parent process */
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Exiting the parent process.

    umask(0); // Change the file mode mask 
    setlogmask (LOG_UPTO (LOG_INFO)); // Open logs here 
    openlog ("Server␣system␣messages:", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL3);
    syslog (LOG_ERR, "Initiating␣new␣server.");

    if (setsid()< 0) { /* Create a new SID for the child process */
    syslog (LOG_ERR, "Error␣creating␣a␣new␣SID␣for␣the␣child␣process.");
    exit(EXIT_FAILURE);
    }

    syslog (LOG_INFO, "Closing␣standard␣file␣descriptors");
    close(STDIN_FILENO); close(STDOUT_FILENO); close(STDERR_FILENO); /* Close out the standard file descriptors */
    return;
}


void sig_int(int signal){
    got_signal=1;
}

void sig_pipe(int signal){
    
}

int main(int argc, char **argv){

    int i;
    char server_root[MAX_SIZE];
    long clients = 10;
    long listen_port = 3000;
    char server_signature[MAX_SIZE];
    struct sigaction act;
    HiloArg *h[MAX_HILOS];


    cfg_opt_t opts[] =
    {
        CFG_STR("server_root", "/",CFGF_NONE),
        CFG_INT("max_clients", 10,CFGF_NONE),
        CFG_INT("listen_port", 8000, CFGF_NONE),
        CFG_STR("server_signature", "Servidor 1.0",CFGF_NONE),
        CFG_END()
    };

    cfg_t *cfg;

    cfg = cfg_init(opts, 0);

    if(cfg_parse(cfg, "server.conf") == CFG_PARSE_ERROR)
        return 1;

    strncpy(server_root, cfg_getstr(cfg,"server_root"),MAX_SIZE);
    clients = cfg_getint(cfg,"max_clients");
    listen_port = cfg_getint(cfg,"listen_port");
    strncpy(server_signature,cfg_getstr(cfg,"server_signature"),MAX_SIZE);

    printf("Parámetros:\n");
    printf("Server_root: %s\n", server_root);
    printf("Max clients: %ld\n",clients);
    printf("listen_port: %ld\n",listen_port);
    printf("server signature: %s\n",server_signature);

    cfg_free(cfg);


    sigemptyset(&(act.sa_mask));
    act.sa_flags=0;
    act.sa_handler=sig_int;

    sigaction(SIGINT, &act, NULL);

    act.sa_handler=sig_pipe;

    sigaction(SIGPIPE, &act, NULL);

    do_daemon();

    listenfd=initiate_server(listen_port,clients);
    nthreads = 10;
    tptr = calloc(nthreads, sizeof(Thread));
    //printf("hoola2\n");
    for (i = 0; i < nthreads; i++){
        h[i] = (HiloArg *)malloc(sizeof(HiloArg));
        h[i]->i = i;
        h[i]->server_root = (char*)malloc(MAX_SIZE);
        h[i]->server_signature = (char*)malloc(MAX_SIZE);
        strncpy(h[i]->server_root,server_root,strlen(server_root)+1);
        strncpy(h[i]->server_signature,server_signature,strlen(server_signature)+1);
        //printf("k\n");
        //printf("los stirngs son %s y %s\n",h[i]->server_root,h[i]->server_signature);

        thread_make((h[i]));
    } 
    //printf("salgo del for\n");

    while(got_signal==0)pause();


    for (i = 0; i < nthreads; i++){
        //printf("Cerrando hilo %d...\n", i);
        pthread_kill(tptr[i].thread_tid, SIGUSR2);
        
    }


    for (i = 0; i < nthreads; i++){
        //printf("Esperando al  hilo %d...\n", i);
        pthread_join(tptr[i].thread_tid, NULL);
        //printf("HA LLEGADO EL %dº DOWN\n", i);

    }
    //printf("CIERROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");

    for (i = 0; i < nthreads; i++){
        
        
        free(h[i]->server_root);
        free(h[i]->server_signature);
        
        free(h[i]);
    } 

    close(listenfd);
    free(tptr);
    
    

}
