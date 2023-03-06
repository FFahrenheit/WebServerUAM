//
// Created by ivan on 2/12/23.
//

#include "../includes/web_server.h"


pthread_mutex_t mutex;
int socket1, socket2;


void handler_SIGINT(int sig) {
    close_socket(socket1);
    close_socket(socket2);

    printf("Sockets cerrados\n");
    exit(0);
}

void run_server(struct WebServer * webServer)
{
    struct Server * server = &webServer->server;

    printf("Server built!\n");
    fflush(stdout);

    //char * hello = "Hello";
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

        socket1 = server->socket;
        socket2 = new_socket;


        procesa_cliente(new_socket, webServer->server_signature, webServer->server_root);

        
       /* while(read(new_socket, buffer, 30000) > 0){
            printf("%s\n", buffer);
            procesa_peticion(new_socket,buffer);
            write(new_socket, hello, strlen(hello));
        }*/
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
    int childpid;
    // Creamos mutex para manejar la pool
    if(pthread_mutex_init(&mutex, NULL) != 0){
        perror("No se pudo crear el mutex");
        exit(1);
    }
    for(int i = 0; i < pool_length; i++)
    {
        if((childpid = fork()) == 0)
        {
            child_handle(i, server);
        }
    }

    while(!!1){};
     */
}

struct WebServer create_server()
{
    struct ServerConfig config = read_config("server.conf");

    struct Server server = server_constructor(
            AF_INET,
            SOCK_STREAM,
            0,
            INADDR_ANY,
            config.port,
            config.max_clients);

    struct WebServer web_server;
    web_server.server = server;
    web_server.launch = run_server;
    web_server.server_root = config.root;
    web_server.server_signature = config.name;

    // printf("PORT:%d|\nBACKLOG:%d|\nNAME:%s|\nROOT:%s|\n", config.port, config.max_clients, web_server.server_signature, web_server.server_root);

    return web_server;
}


/*
void child_handle_fork(int new_socket)
{
    char buffer[30000];
    char * hello = "Hello!";
    read(new_socket, buffer, 30000);
    //printf("%s\n", buffer);

    write(new_socket, hello, strlen(hello));
    close(new_socket);
}

void child_handle(int pool_id, struct Server * server)
{
    char buffer[30000];
    char * hello = "HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: Apache/2.2.14 (Win32)\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nContent-Length: 88\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
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
        //printf("%s\n", buffer);

        write(new_socket, hello, strlen(hello));
        close(new_socket);
    }
}
*/