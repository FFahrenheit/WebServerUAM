#ifndef PETICIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

struct ServerConfig
{
    int port;
    int max_clients;
    char * root;
    char * name;
};

//declaraciones para el mapeo de extensiones 
typedef struct {
  char* extension;
  char* etiqueta;
}Extension;

char * get_time(char * path);
char *substring(const char *str, int start, int end);
int read_key(char* input, char** left, char** right);
int file_exists(char* path);

#define PETICIONES_H
#endif //PETICIONES_H

