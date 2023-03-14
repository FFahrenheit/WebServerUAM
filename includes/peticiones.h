#ifndef PETICIONES_H
#include "../includes/picohttpparser.h"
#include "../includes/http_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

/**
 * @brief esta estructura controla los diferentes parametros de la request(util pra el picoparser)
 *
 */
typedef struct
{
    const char *method;            // metodo de la request
    size_t method_len;             // longitud del metodo de la req
    const char *path;              // path de la request
    size_t path_len;               // longitud del path
    int minor_version;             // version del http
    struct phr_header headers[50]; // headers de la request
    size_t num_headers;            // numero de headers
    char *body;                    // body de la request

} PeticionHTTP;

/**
 * @brief esta funcion recoge en un buffer el contenido de read de un cliente  y controla los parametros para si
 * posterior procesamiento
 *
 * @param socket_fd el socket del cliente
 * @param server_name nombre del serveidor
 * @param server_root root del serveidor
 * @return 0 OK; 1 ERROR
 */
int procesa_cliente(int socket_fd, char *server_name, char *server_root);

/**
 * @brief esta funcion se ocupa de leer, parsear y organizar la request y diversifica el trabajo segun el tipo 
 * de peticion (GET, POST, OPTIONS o error)
 *
 * @param socket_fd socket del cliente
 * @param server_name nombre del server
 * @param server_root root del server
 * @param request un buffer con toda la informacion de la request sin organizar
 * @param read_len longitud del buffer
 * @return 0 OK,1 ERROR
 */
int procesa_peticion(int socket_fd, char *server_name, char *server_root, char *request, int read_len);

/**
 * @brief se ocupa de procesar una peticion GET y genera la response correspondiente
 *
 * @param socket_fd socket del cliente
 * @param req request organizada
 * @param path path de la peticion
 * @param server_name nombre del server
 * @param query_params parametros de la query(puede no existir)
 * @return 0 OK, 1 ERROR
 */
int procesa_get(int socket_fd, PeticionHTTP *req, char *path, char *server_name, char *query_params);

/**
 * @brief esta función se ocupa de generar una response para el metodo POST
 *
 * @param socket_fd socket del cliente
 * @param req request organizada
 * @param path path de la peticion
 * @param body body de la request
 * @param server_name nombre del server
 * @return 0 OK; 1 ERROR
 */
int procesa_post(int socket_fd, PeticionHTTP *req, char *path, char *body, char *server_name);

/**
 * @brief esta funcion se ocupa de generar una response para OPTIONS
 *
 * @param socket_fd socket del cliente
 * @param req request organizada
 * @param path path de la peticion
 * @param server_name nombre del servidor
 * @param real_path server_root + path (ubicacion en disco)
 * @return 0 OK; 1 ERROR
 */
int procesa_options(int socket_fd, PeticionHTTP *req, char *path, char *server_name, char *real_path);

/**
 * @brief Genera el codigo de error necesario para cada caso y lo manda como response
 *
 * @param socket_fd socket del cliente
 * @param req request organizada
 * @param error error y codigo en cuestion
 * @param server_name nombre del server
 * @return 1 (codigo de error)
 */
int error_response(int socket_fd, PeticionHTTP *req, char *error, char *server_name);

#define PETICIONES_H
#endif // PETICIONES_H
