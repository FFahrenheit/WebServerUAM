//
// Created by ivan on 2/12/23.
//


#include "server.h"


struct Server server_constructor(int domain, int service, int protocol, unsigned long int interface, int port, int backlog)
{
    struct Server server;

    //Asignamos las propiedades del servidor
    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port); //Convertir de int a orden de red TCP/IP (big-endian)
    server.address.sin_addr.s_addr = htonl(interface); //Convertir de long a orden de red TCP/IP

    //Creamos el Socket para aceptar conexiones
    server.socket = open_socket(domain, service, protocol);


    if((bind(server.socket,
            (struct sockaddr *) &server.address, //Cast a socketaddr *
            sizeof(server.address))) < 0)
    {
        perror("No se pudo hacer bind correctamente");
        exit(1);
    }

    // Escuchamos en el socket hasta backlog clientes
    if( (listen(server.socket, server.backlog)) < 0)
    {
        perror("No se pudo escuchar");
        exit(1);
    }

    return server;
}


int open_socket(int domain, int service, int protocol){

    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socket_fd <0){
        fprintf(stderr, "ERROR, al abrir server");
        exit(1);
    }
      
    if(socket_fd == 0)
    {
        perror("No se pudo conectar el socket");
        exit(1);
    }
    return socket_fd;
}
int bind_socket(int socket_fd, int puerto);
int accept_conn(int server_fd);

void close_socket(int socket_fd){
    close(socket_fd);
    return;
}

