//
// Created by ivan on 2/12/23.
//

#include "web_server.h"
#include "peticiones.h"


pthread_mutex_t mutex;
int socket1, socket2;


void handler_SIGINT(int sig) {
    close_socket(socket1);
    
    close_socket(socket2);
    printf("todo cerrado\n");
    exit(0);
}

/*
 * Pruebas: 100 veces * 100 requests
 * Iterativo: 22.74
 * Reactivo: 49.96s
 * Pool: 22.53
 */
void run_server(struct WebServer * webServer)
{
    struct Server * server = &webServer->server;
    printf("Server built!\n");
    fflush(stdout);

    char buffer[30000];
    char * hello = "Hello!";
    int address_length = sizeof(server->address);
    int new_socket;
    struct sigaction act1;

    //Iterativo
    socket1 = 0;
    socket2 = 0;


    act1.sa_handler = handler_SIGINT;
    sigemptyset(&(act1.sa_mask));
    sigaddset(&(act1.sa_mask), SIGINT);
    act1.sa_flags = 0;

    if (sigaction(SIGINT, &act1, NULL) < 0) {
        perror("sigint");
        exit(EXIT_FAILURE);
    }


    while(!0)
    {
        printf("----- ESPERANDO CONEXION -----\n");

        new_socket = accept(server->socket,
                            (struct sockaddr *) &server->address,
                            (socklen_t *) &address_length);

        int read_len;

        while((read_len = read(new_socket, buffer, 30000)) > 0){

            procesa_peticion(new_socket, buffer, read_len, server);
            // write(new_socket, hello, strlen(hello));
            // printf("Sent response\n");
        }
        close(new_socket);
        printf("Conexion cerrada\n");
    }

    //Reactivo
    /*
    while(!0)
    {
        int childpid;
        printf("----- ESPERANDO CONEXION -----\n");

        new_socket = accept(server->socket,
                            (struct sockaddr *) &server->address,
                            (socklen_t *) &address_length);

        if( (childpid = fork()) == 0)
        {
            printf("fork creado\n");
            child_handle_fork(new_socket);
            printf("fork finalizado");
        }
    }
     */

    //Pool
    /*
    int pool_length = 5;
    // Creamos mutex para manejar la pool
    if(pthread_mutex_init(&mutex, NULL) != 0){
        perror("No se pudo crear el mutex");
        exit(1);
    }
    for(int i = 0; i < pool_length; i++)
    {
        int childpid;
        if((childpid = fork()) == 0)
        {
            child_handle(i, server);
        }
    }

    while(!!1){};
     */
}

void child_handle_fork(int new_socket)
{
    char buffer[30000];
    char * hello = "Hello!";
    read(new_socket, buffer, 30000);
    printf("%s\n", buffer);

    write(new_socket, hello, strlen(hello));
    close(new_socket);
}

void child_handle(int pool_id, struct Server * server)
{
    char buffer[30000];
    char * hello = "Hello!";
    int address_length = sizeof(server->address);
    int new_socket;

    while(1)
    {
        printf("----- ESPERANDO CONEXION %i -----\n", pool_id);

        pthread_mutex_lock(&mutex);
        new_socket = accept(server->socket,
                            (struct sockaddr *) &server->address,
                            (socklen_t *) &address_length);
        pthread_mutex_unlock(&mutex);

        read(new_socket, buffer, 30000);
        printf("%s\n", buffer);

        write(new_socket, hello, strlen(hello));
        close(new_socket);
    }
}

struct WebServer create_server(int port, int backlog)
{
    struct Server server = server_constructor(
            AF_INET,
            SOCK_STREAM,
            0,
            INADDR_ANY,
            port,
            backlog);

    struct WebServer web_server;
    web_server.server = server;
    web_server.launch = run_server;

    return web_server;
}