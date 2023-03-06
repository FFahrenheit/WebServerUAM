#ifndef PETICIONES_H

#include "../includes/picohttpparser.h"
#include "../includes/http_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

//#include "../includes/server.h"


typedef struct
{
    const char *method;
    size_t method_len;
    const char *path;
    size_t path_len;
    int minor_version;
    struct phr_header headers[50];
    size_t num_headers;
    char *body;

}PeticionHTTP;

//int procesa_peticion(int socket_fd, char* server_name, char* server_root);
int procesa_cliente(int socket_fd, char * server_name, char * server_root);
int procesa_peticion(int socket_fd, char * server_name, char * server_root, char * request, int read_len);
int error_response(int socket_fd, PeticionHTTP * req, char * error, char * server_name);

int procesa_get(int socket_fd, PeticionHTTP * req, char* path,char* server_name);
int procesa_post(int socket_fd,PeticionHTTP * req, char* path, char* body,char* server_name);
int procesa_options(int socket_fd, PeticionHTTP * req, char* path,char* server_name,  char* real_path);

char * get_time(char * path);

#define PETICIONES_H
#endif //PETICIONES_H

