//
// Created by ivan on 2/12/23.
//

#ifndef HTTPSERVER_WEBSERVER_H
#define HTTPSERVER_WEBSERVER_H

#include "server.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>


struct WebServer 
{
    struct Server server;
    void (*launch)(struct WebServer * server);
};

void child_handle_fork(int new_socket);
void child_handle(int pool_id, struct Server * server);
void launch_server(struct Server * server);
struct WebServer create_server(int port, int backlog);

#endif //HTTPSERVER_WEBSERVER_H
