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

/* estructura central del servidor web */
struct WebServer
{
    struct Server server;                       //server al uso con la informacion necesaria(declarada en server.h)
    void (*launch)(struct WebServer *server);   //referencia a la funcion run_server
    char *server_root;                          //root del server
    char *server_signature;                     //nombre del server
};

/**
 * @brief funcion para la creacion del web_server usando server.h, seleccionando los valores
 * adecuados para la construccion del servidor (como tipo de socket, etc.)
 * 
 * @return la estructura central web_server
 */
struct WebServer create_server();

/**
 * @brief funcion que administra los procesos para el servidor reactivo
 * 
 * @param new_socket socket de la peticion a procesar en el thread
 */
//void child_handle_fork(int new_socket);

/**
 * @brief funcion que administra los procesos para el servidor de hilos precreados
 * 
 * @param pool_id id del hilo a administrar
 * @param webServer estructura webserver
 */
void child_handle(int pool_id, struct WebServer *webServer);

/**
 * @brief Funcion que maneja la senal de ctrl + c
 * 
 * @param sig ID de la senal recibida
 */
void handler_SIGINT(int sig);

/**
 * @brief Funcion que inicia a correr el servidor http, crea los threads en cuestion
 * e inicializa la lectura
 * 
 * @param webServer estructura del servidor en cuestion
 */
void run_server(struct WebServer *webServer);

#endif // HTTPSERVER_WEBSERVER_H
