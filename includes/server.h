//
// Created by ivan on 2/12/23.
//

#ifndef HTTPSERVER_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
/* estructura server donde se guarda la informacion necesaria */
struct Server
{
    int domain;                 //familia de direcciones permitidas (AF_INET)
    int service;                //tipo de socket permitido (SOCK_STREAM)
    int protocol;               //tipo de protocolos permitido
    unsigned long int interface;//IPS permitidas
    int port;                   //puerto de escucha del server
    int backlog;                // Cantidad de usuarios en espera
    struct sockaddr_in address; // Struct del socket
    int socket;                 //socket del servidor
};

/**
 * @brief constructor del servidor
 * 
 * @param domain familia de direcciones permitidas
 * @param service tipo de socket permitido
 * @param protocol tipo de protocolos permitidos
 * @param interface IPS permitidas
 * @param port Puerto de escucha del server 
 * @param backlog Cantidad de usuarios en espera
 * @return Struct Server para ya completa y construida-> OK; NULL-> ERROR 
 */
struct Server server_constructor(int domain, int service, int protocol, unsigned long int interface, int port, int backlog);

/**
 * @brief funcion que se ocurra de abrir el socket
 * 
 * @param domain familia de direcciones permitidas, necesario para socket()
 * @param service tipo de socket permitido, necesario para socket()
 * @param protocol tipo de protocolo permitido, necesario para socket()
 * @return socket_fd del socket abierto -> OK; -1 -> ERROR
 */
int open_socket(int domain, int service, int protocol);

/**
 * @brief funcion que se ocupa del bind dek socket
 * 
 * @param socket fd del socket a "bindear"
 * @param address address del socket
 * @param size tamanyo
 * @return 0 OK ; -1 ERROR
 */
int bind_socket(int socket, struct sockaddr *address, int size);

/**
 * @brief funcion que se ocupa a cerrar el socket
 * 
 * @param socket_fd fd del socket a cerrar
 */
void close_socket(int socket_fd);

#define HTTPSERVER_SERVER_H
#endif // HTTPSERVER_SERVER_H
