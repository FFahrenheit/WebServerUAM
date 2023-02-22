#ifndef PETICIONES_H

#include "picohttpparser.h"
#include "server.h"


struct PeticionHTTP
{

};

int procesa_peticion(int socket, char * buffer, int read_len, struct Server * server);
void parse_headers(char * request, int read_len);

#define PETICIONES_H
#endif //PETICIONES_H
