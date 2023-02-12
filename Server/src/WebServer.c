//
// Created by ivan on 2/12/23.
//

#include "../include/WebServer.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex;

/*
 * Pruebas: 100 veces * 100 requests
 * Iterativo: 22.74
 * Reactivo: 49.96s
 * Pool: 22.53
 */
void launch_server(struct Server * server)
{
    printf("Server built!\n");
    fflush(stdout);

    char buffer[30000];
    char * hello = "Hello!";
    int address_length = sizeof(server->address);
    int new_socket;
    int childpid;

    //Iterativo

    while(!0)
    {
        printf("----- ESPERANDO CONEXION -----\n");

        new_socket = accept(server->socket,
                            (struct sockaddr *) &server->address,
                            (socklen_t *) &address_length);

        read(new_socket, buffer, 30000);
        printf("%s\n", buffer);

        write(new_socket, hello, strlen(hello));
        close(new_socket);
    }


    //Reactivo
    /*
    while(!0)
    {
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
            backlog,
            launch_server);

    struct WebServer web_server;
    web_server.server = server;
    web_server.launch = launch_server;

    return web_server;
}