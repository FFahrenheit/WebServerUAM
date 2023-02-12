//
// Created by ivan on 2/12/23.
//

#ifndef HTTPSERVER_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

struct Server
{
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog; //Cantidad de usuarios en espera
    struct sockaddr_in address; //Struct del socket
    int socket;

    void (*launch)(struct Server * server);//Funcion callback
};

struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog, void (*launch)(struct Server * server));

#define HTTPSERVER_SERVER_H
#endif //HTTPSERVER_SERVER_H
