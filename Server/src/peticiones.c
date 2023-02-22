#include "peticiones.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include "picohttpparser.h"

// nc -C -v 192.168.11.129 8080

 int procesa_peticion(int client_socket, char * buffer, int read_len, struct Server * server){
   //printf("%s\n", buffer);
   parse_headers(buffer, read_len);

   char * dummy = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 48\nContent-Type: text/html\nConnection: Close\n\n<html><body><h1>Hello, World!</h1></body></html>";
   write(client_socket, dummy, strlen(dummy));
   
   
   printf("End\n");
   return -1;
}

void parse_headers(char * request, int read_len)
{
   printf("Size of request: %d\n", sizeof(request));
   const char *method;
   size_t method_len;
   const char *path;
   size_t path_len;
   int minor_version;
   struct phr_header headers[32];
   size_t num_headers;
   int ret;

   num_headers = sizeof(headers) / sizeof(headers[0]);
   ret = phr_parse_request(request, read_len, &method, &method_len, &path, &path_len, &minor_version, headers, &num_headers, 0);

   printf("Method is %.*s\n", method_len, method);
   printf("Path is %.*s\n", path_len, path);
   printf("Minor version is %d\n", minor_version);
   printf("%zu headers found\n", num_headers);

   for(int i = 0; i < num_headers; i++)
   {
      struct phr_header current = headers[i];
      printf("\t%.*s : %.*s\n", current.name_len, current.name, current.value_len, current.value);
   }

   return 0;
}