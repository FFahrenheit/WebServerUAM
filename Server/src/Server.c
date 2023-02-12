//
// Created by ivan on 2/12/23.
//

#include <stdlib.h>
#include <stdio.h>
#include "../include/Server.h"

struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog,
        void (*launch)(struct Server * server)){
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
    server.socket = socket(domain, service, protocol);
    if(server.socket == 0)
    {
        perror("No se pudo conectar el socket");
        exit(1);
    }

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

    server.launch = launch;
    return server;
}