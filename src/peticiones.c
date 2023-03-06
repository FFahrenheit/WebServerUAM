#include "../includes/peticiones.h"

#define TAM_BUFFER 30000
#define TAM_STR 200
#define TAM_CHUNK 256


Extension EXTENSIONES [] = {
    {".txt", "text/plain"},
    {".html", "text/html"},
    {".htm", "text/html"},
    {".ico", "image/x-icon"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpeg", "image/jpeg"},
    {".jpg", "image/jpeg"}, 
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".doc", "application/msword"},
    {".docx", "application/msword"},
    {".mp3", "audio/mpeg"},
    {".pdf", "application/pdf"},
    {".mp4", "video/mp4"},
    {NULL, NULL},
};

int procesa_cliente(int socket_fd, char * server_name, char * server_root)
{
  int read_len;
  char buffer[30000];

  while((read_len = read(socket_fd, buffer, 30000)) > 0)
  {
    procesa_peticion(socket_fd, server_name, server_root, buffer, read_len);
  }

  return 1;
}

int procesa_peticion(int socket_fd, char * server_name, char * server_root, char * request, int read_len)
{
  //printf("%d %s SERVER ROOT\n", strlen(server_root) ,server_root);
  PeticionHTTP *req;
  int ret_parseo, ctrl;
  size_t last_len;
  char method[TAM_STR], path[TAM_STR], result[TAM_STR];

  last_len = 0;

  req = malloc(sizeof(PeticionHTTP));
  req->num_headers = sizeof(req->headers) / sizeof(req->headers[0]);
  ret_parseo = phr_parse_request(request, read_len, &(req->method), &(req->method_len), &(req->path), &(req->path_len), &(req->minor_version), req->headers, &(req->num_headers), last_len);

  if(ret_parseo == -1)
  {
    // BAD REQUEST (malformada)
    return error_response(socket_fd, req, "400 Bad Request", server_name);
  }
  else if(ret_parseo == -2)
  {
    // REQUEST TOO BIG
    return error_response(socket_fd, req, "431 Request Header Fields Too Large", server_name);
  }

  //printf("RETORNO : %d\n", ret_parseo);
  sprintf(method, "%.*s", (int)req->method_len, req->method);
  sprintf(path, "%.*s", (int)req->path_len, req->path);

  char* query_params;

  // Get real resource
  sprintf(path, "%s", strtok(path, "?"));
  // Get query params (if any)
  query_params = strtok(NULL, "?");

  printf("Path: |%s|, Query params: |%s|\n", path, query_params);
  
  if(strcmp(path, "/") == 0 && req->path_len == 1 && strcmp(method, "GET") == 0) //Caso GET al servidor
  {
    sprintf(path, "/index.html");
    printf("MAIN ROOT == INDEX\n");
  }

  printf("METHOD : |%.*s|\n", (int) req->method_len, req->method);
  printf("PATH : |%.*s|\n", (int) req->path_len, req->path);
  printf("MINOR : |%d|\n", req->minor_version);
  printf("SERVER ROOT : |%s|\n", server_root);

  // Leer headers
  char header[TAM_STR], value[TAM_STR];
  int body_lenght = 0;

  for(int i = 0; i < req->num_headers; i++)
  {
    struct phr_header current = req->headers[i];
    sprintf(header, "%.*s", (int) current.name_len, current.name);
    sprintf(value, "%.*s", (int) current.value_len, current.value);

    printf("\t%s : %s\n", header, value);

    if(strcmp(header, "Content-Length") == 0)
    {
      body_lenght = atoi(value);
      printf("Body length = %d\n", body_lenght);
    }
  }

  char * body = substring(request, ret_parseo, body_lenght + ret_parseo);
  printf("Body: |%s|\n", body);

  sprintf(result, "%s%s", server_root, path);
  printf("Real path: %s\n", result);

  if (strcmp(method, "GET") == 0)
  {
    printf("GET REQUEST\n");
    ctrl = procesa_get(socket_fd, req, result, server_name);
  }
  else if (strcmp(method, "POST") == 0)
  {
    printf("POST REQUEST\n");
    //ctrl = procesa_post(socket_fd, req, path, body, server_name);
    ctrl = error_response(socket_fd, req, "405 Method Not Allowed", server_name);
  }
  else if (strcmp(method, "OPTIONS") == 0)
  {
    printf("OPTIONS REQUEST\n");
    ctrl = procesa_options(socket_fd, req, result, server_name, path);
  }
  else
  {
    ctrl = error_response(socket_fd, req, "405 Method Not Allowed", server_name);
  }

  free(req);
  free(body);

  printf("END OF REQUEST\n");
  return ctrl;
}

int procesa_get(int socket_fd, PeticionHTTP * req, char * path, char * server_name)
{
  printf("GET request TO %s\n", path);

  if (file_exists(path) != 0)
  {
    return error_response(socket_fd, req, "404 Not Found", server_name);
  }

  char buffer[TAM_BUFFER] = {0}, response[TAM_BUFFER] = {0}, ext_aux[TAM_STR] = {0};
  char *ext;
  Extension  * extensiones = EXTENSIONES;

  FILE * get_file;

  ext = strrchr(path, '.');
  int found = 0;
  if (ext == NULL) 
  {
      sprintf(ext_aux, "text/plain");
  }
  else
  {
      while (extensiones->extension)
      {
          if (strcmp(extensiones->extension, ext) == 0)
          {
              sprintf(ext_aux, "%s", extensiones->etiqueta);
              found = 1;
              break;
          }
          extensiones++;
      }
      if(found == 0)
      {
        sprintf(ext_aux, "text/plain");
      }
  }

  printf("CONTENT TYPE: %s\n", ext_aux);

  // ENVIO DE UN ARCHIVO
  get_file = fopen(path, "rb");
  printf("REAL PATH %s\n", path);

  // obtener la longitud del archivo
  fseek(get_file, 0, SEEK_END);
  long int file_size = ftell(get_file);
  fseek(get_file, 0, SEEK_SET);


  // Montaje de la response
  sprintf(response,"HTTP/1.%d 200 OK\r\n", req->minor_version);
  strcat(buffer,response);
  
  char * now = get_time(NULL);
  sprintf(response, "Date: %s\r\n", now);
  free(now);
  
  strcat(buffer,response);
  sprintf(response,"Server: %s\r\n", server_name);
  strcat(buffer,response);

  char * modified_time = get_time(path);
  sprintf(response,"Last-Modified: %s\r\n", modified_time);
  free(modified_time);

  strcat(buffer,response);
  sprintf(response,"Content-Length: %ld\r\n",file_size);
  strcat(buffer,response);
  sprintf(response,"Content-Type: %s\r\n", ext_aux);
  strcat(buffer,response);
  sprintf(response, "Connection: Keep-Alive\r\n");
  strcat(buffer,response);
  sprintf(response, "Keep-Alive: timeout=2, max=20\r\n");
  strcat(buffer, response);
  sprintf(response, "\r\n");
  strcat(buffer,response);

  // enviar los encabezados HTTP
  send(socket_fd, buffer, strlen(buffer), 0);
  
  // leer el archivo en chunks y enviarlos
  int remaining = file_size;

  while(remaining > 0)
  {
    int size = remaining > TAM_CHUNK ? TAM_CHUNK : remaining;
    char * file_buffer = malloc(size);
    fread(file_buffer, size, 1, get_file);
    send(socket_fd, file_buffer, size, 0);

    remaining -= size;
    free(file_buffer);
  }
  fclose(get_file);

  return 0;
}


int procesa_post(int socket_fd, PeticionHTTP *req, char *path, char *body, char *server_name)
{
  return 0;
}


int procesa_options(int socket_fd, PeticionHTTP *req, char *path, char *server_name, char* real_path)
{
  if(file_exists(path) != 0)
  {
    return error_response(socket_fd, req, "404 Not Found", server_name);
  }

  char buffer[TAM_BUFFER] = {0}, response[TAM_BUFFER] = {0};
  char* now = get_time(NULL);
  char* allowed;

  printf("OPTIONS PATH: %s\n", path);

  if(strstr(path, "/scripts/") != NULL)
  {
    allowed = "GET, POST, OPTIONS";
  }
  else if(strcmp(real_path, "/") == 0)
  {
    allowed = "GET, POST, OPTIONS";
  }
  else
  {
    allowed = "GET, OPTIONS";
  }

  sprintf(response, "HTTP/1.%d 200 OK\r\n", req->minor_version);
  strcat(buffer, response);
  sprintf(response, "Date: %s\r\n", now);
  strcat(buffer, response);
  sprintf(response, "Server: %s\r\n", server_name);
  strcat(buffer, response);
  sprintf(response, "Allow: %s\r\n", allowed); // TO be changed depending on route
  strcat(buffer, response);
  sprintf(response, "Content-Length: 0\r\n");
  strcat(buffer, response);  
  sprintf(response, "Connection: Close\r\n");
  strcat(buffer, response);
  sprintf(response, "\r\n");
  strcat(buffer, response);

  send(socket_fd, buffer, strlen(buffer), 0);

  free(now);
  return 0;
}

int error_response(int socket_fd, PeticionHTTP * req, char * error, char * server_name)
{
  printf("ERROR: %s\n", error);
  char buffer[TAM_BUFFER] = {0}, response[TAM_BUFFER] = {0}, content[TAM_BUFFER] = {0};
  
  sprintf(content,
    "%s%s%s%s%s%s%s", 
    "<html><head><title>", 
    error,
    "</title></head><body bgcolor='white'><center><h1>",
    error,
    "</h1></center><hr><center>",
    server_name,
    "</center></body></html>");

  sprintf(response, "HTTP/1.%d %s\r\n", req->minor_version, error);
  strcat(buffer, response);
  
  char* now = get_time(NULL);
  sprintf(response, "Date: %s\r\n", now);
  free(now);

  strcat(buffer, response);
  sprintf(response, "Server: %s\r\n", server_name);
  strcat(buffer, response);
  sprintf(response, "Content-Length:%ld\r\n", strlen(content));
  strcat(buffer, response);

  // Depending on file
  sprintf(response, "Content-Type: text/html\r\n");
  strcat(buffer, response);
  
  sprintf(response, "Connection: Close\r\n");
  strcat(buffer, response);
  sprintf(response, "\r\n");
  strcat(buffer, response);
  sprintf(response, "%s", content);
  strcat(buffer, response);   
  write(socket_fd, buffer, strlen(buffer));

  return 1;
}