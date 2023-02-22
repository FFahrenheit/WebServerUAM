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

struct Server
{
    int domain;
    int service;
    int protocol;
    unsigned long int interface;
    int port;
    int backlog; //Cantidad de usuarios en espera
    struct sockaddr_in address; //Struct del socket
    int socket;

};

struct Server server_constructor(int domain, int service, int protocol, unsigned long int interface, int port, int backlog);
int open_socket();
int bind_socket(int socket_fd, int puerto);
int accept_conn(int server_fd);
void close_socket(int socket_fd);


#define HTTPSERVER_SERVER_H
#endif //HTTPSERVER_SERVER_H
