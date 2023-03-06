//
// Created by ivan on 2/12/23.
//

#ifndef HTTPSERVER_WEBSERVER_H
#define HTTPSERVER_WEBSERVER_H


#include "../includes/server.h"
#include "../includes/peticiones.h"
#include "../includes/http_utils.h"

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
    char * server_root;
    char * server_signature;
};

struct ServerConfig read_config(char * filename);
struct WebServer create_server();

//void child_handle_fork(int new_socket);
//void child_handle(int pool_id, struct Server * server);

#endif //HTTPSERVER_WEBSERVER_H
