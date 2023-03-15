#include "../includes/peticiones.h"

#define TAM_BUFFER 30000 // tamaño de buffer
#define TAM_STR 256      // tamaño de string
#define TAM_CHUNK 256    // tamaño de chunk

/*tipo de extensiones y MIME type permitidas por el servidor*/
Extension EXTENSIONES[] = {
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
    {".py", "text/plain"},
    {".php", "text/plain"},
    {NULL, NULL},
};

/* esta funcion recoge en un buffer el contenido de read y controla los parametros para el procesamiento*/
int procesa_cliente(int socket_fd, char *server_name, char *server_root)
{
  int read_len, ctrl = 1;
  char buffer[TAM_BUFFER];
  
  // control de errores de la entrada de la funcion
  if (!server_name || !server_root || socket_fd < 0)
  {
    fprintf(stderr, "ERROR en los argumentos de entrada procesa_cliente\n");
    return ctrl;
  }

  while ((read_len = read(socket_fd, buffer, TAM_BUFFER)) > 0)
  {
    // una vez leido procesa la peticion
    ctrl = procesa_peticion(socket_fd, server_name, server_root, buffer, read_len);
  }
  // retorna la variable de control
  return ctrl;
}

/*organiza la request y llama al metodo necesario*/
int procesa_peticion(int socket_fd, char *server_name, char *server_root, char *request, int read_len)
{
  printf("[%d] %s\n", read_len, request);
  PeticionHTTP *req;
  char method[TAM_STR], path[TAM_STR], result_path[TAM_STR];

  int ret_parseo, ctrl = 1;
  size_t last_len = 0;
  char *query_params;

  if (!server_name || !server_root || socket_fd < 0 || read_len < 1 || !request)
  {
    fprintf(stderr, "ERROR en los argumentos de entrada procesa_peticion\n");
    return ctrl;
  }

  // req tendra la info de la request

  req = malloc(sizeof(PeticionHTTP));
  req->num_headers = sizeof(req->headers) / sizeof(req->headers[0]);
  ret_parseo = phr_parse_request(request, read_len, &(req->method), &(req->method_len), &(req->path), &(req->path_len), &(req->minor_version), req->headers, &(req->num_headers), last_len);

  // controlamos el parseo y su retorno, si error se llama error_responswe
  if (ret_parseo == -1)
  {
    // BAD REQUEST (malformada)
    ctrl = error_response(socket_fd, req, "400 Bad Request", server_name);
    free(req);
    return ctrl;
  }
  else if (ret_parseo == -2 && sizeof(request) == read_len)
  {
    // REQUEST demasiado grande
    ctrl = error_response(socket_fd, req, "431 Request Header Fields Too Large", server_name);
    free(req);
    return ctrl;
  }

  sprintf(method, "%.*s", (int)req->method_len, req->method);
  sprintf(path, "%.*s", (int)req->path_len, req->path);

  // Obtener el recurso en cuestion
  strtok(path, "?");
  // Y sus query params (si los hay)
  query_params = strtok(NULL, "?");

  //  printf("Path: |%s|, Query params: |%s|\n", path, query_params);

  if (strcmp(path, "/") == 0 && strcmp(method, "GET") == 0) // Caso GET al servidor
  {
    sprintf(path, "/index.html");
  }

  //  printf("METHOD : |%.*s|\n", (int) req->method_len, req->method);
  //  printf("PATH : |%.*s|\n", (int) req->path_len, req->path);
  //  printf("MINOR : |%d|\n", req->minor_version);
  //  printf("SERVER ROOT : |%s|\n", server_root);

  // Leer headers
  char header[TAM_STR], value[TAM_STR];
  int body_lenght = 0;

  for (int i = 0; i < req->num_headers; i++)
  {
    struct phr_header current = req->headers[i];
    sprintf(header, "%.*s", (int)current.name_len, current.name);
    sprintf(value, "%.*s", (int)current.value_len, current.value);

    // printf("\t%s : %s\n", header, value);

    // Obtenemos la longitud del contenido en la peticion y la guardamos para leerla posteriormente
    if (strcmp(header, "Content-Length") == 0)
    {
      body_lenght = atoi(value);
    }
  }

  // Leemos el body, si la longitud es 0 sera NULL (comportamiento esperado)
  char *body = substring(request, ret_parseo, body_lenght + ret_parseo);
  //  printf("Body: |%s|\n", body);

  sprintf(result_path, "%s%s", server_root, path);

  // con el metodo ya leido se selecciona el tipo de funcion

  if (strcmp(method, "GET") == 0)
  {
    // llamada a procesar GET
    ctrl = procesa_get(socket_fd, req, result_path, server_name, query_params);
  }
  else if (strcmp(method, "POST") == 0)
  {
    // llamada a procesar POST
    ctrl = procesa_post(socket_fd, req, result_path, body, server_name);
  }
  else if (strcmp(method, "OPTIONS") == 0)
  {
    // llamada a procesar OPTIONS
    ctrl = procesa_options(socket_fd, req, result_path, server_name, path);
  }
  else
  {
    // Error 405, el metodo no esta implementado
    ctrl = error_response(socket_fd, req, "405 Method Not Allowed", server_name);
  }
  // libera los recursos
  free(req);
  free(body);

  printf("Peticion [%s] a [%s] con respuesta [%d]\n", method, path, ctrl);
  return ctrl;
}

/*genera una respuesta get*/
int procesa_get(int socket_fd, PeticionHTTP *req, char *path, char *server_name, char *query_params)
{
  char buffer[TAM_BUFFER] = {0}, response[TAM_BUFFER] = {0}, ext_aux[TAM_STR] = {0};
  char *ext;
  Extension *extensiones = EXTENSIONES;
  FILE *get_file;
  int found = 0, is_script, ctrl = 1;
  long int file_size;
  char *script_output;

  // control de errores de entrada
  if (socket_fd < 0 || !req || !server_name)
  {
    fprintf(stderr, "ERROR en los argumentos de entrada procesa_get\n");
    return ctrl;
  }

  // Si no existe... 404
  if (file_exists(path) != 0)
  {
    ctrl = error_response(socket_fd, req, "404 Not Found", server_name);
    return ctrl;
  }

  // Si no es directorio... 403
  if (is_regular_file(path) != 1)
  {
    return error_response(socket_fd, req, "403 Forbidden", server_name);
  }

  // Obtenemos la extension
  ext = strrchr(path, '.');
  // Verificamos si es un script
  is_script = (strcmp(ext, ".py") == 0 || strcmp(ext, ".php") == 0) ? 1 : 0;

  // control de las extensiones y MIME types
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
    if (found == 0)
    {
      sprintf(ext_aux, "text/plain");
    }
  }

  //  printf("CONTENT TYPE: %s\n", ext_aux);

  // funcionamiento para los scripts
  if (is_script == 1)
  {
    // correr script, obtenemos interprete
    char *lang = strcmp(ext, ".py") == 0 ? "python3" : "php";

    script_output = run_script(lang, path, query_params);
    file_size = strlen(script_output);

    // printf("out: %s\n\tLength: %ld", script_output, file_size);
  }
  else
  {

    // Si el archivo no existe
    get_file = fopen(path, "rb");
    if (!get_file)
    {
      fprintf(stderr, "ERROR abriendo el archivo\n");
      return error_response(socket_fd, req, "500 Internal Server Error", server_name);
    }


    // Obtenemos la longitud
    fseek(get_file, 0, SEEK_END);
    file_size = ftell(get_file);
    fseek(get_file, 0, SEEK_SET);
  }

  // Montaje de la response
  sprintf(response, "HTTP/1.%d 200 OK\r\n", req->minor_version);
  strcat(buffer, response);

  char *now = get_time(NULL);
  sprintf(response, "Date: %s\r\n", now);
  free(now);

  strcat(buffer, response);
  sprintf(response, "Server: %s\r\n", server_name);
  strcat(buffer, response);

  char *modified_time = get_time(path);
  sprintf(response, "Last-Modified: %s\r\n", modified_time);
  free(modified_time);

  strcat(buffer, response);
  sprintf(response, "Content-Length: %ld\r\n", file_size);
  strcat(buffer, response);
  sprintf(response, "Content-Type: %s\r\n", ext_aux);
  strcat(buffer, response);
  sprintf(response, "Connection: Keep-Alive\r\n");
  strcat(buffer, response);
  sprintf(response, "Keep-Alive: timeout=5, max=20\r\n");
  strcat(buffer, response);
  sprintf(response, "\r\n");
  strcat(buffer, response);

  // enviar los encabezados HTTP
  send(socket_fd, buffer, strlen(buffer), 0);

  // leer el archivo en chunks y enviarlos
  if (is_script == 1)
  {
    send(socket_fd, script_output, file_size, 0);
    free(script_output);
  }
  else
  {
    int remaining = file_size;

    // Se envia por chunks el archivo
    while (remaining > 0)
    {
      int size = remaining > TAM_CHUNK ? TAM_CHUNK : remaining;
      char *file_buffer = malloc(size);
      fread(file_buffer, size, 1, get_file);
      send(socket_fd, file_buffer, size, 0);

      remaining -= size;
      free(file_buffer);
    }
    fclose(get_file);
  }

  return 0;
}
/* esta función se ocupa de generar una response para el metodo post*/
int procesa_post(int socket_fd, PeticionHTTP *req, char *path, char *body, char *server_name)
{
  char *ext = strrchr(path, '.');

  // Si es un directorio... 403
  if(ext == NULL || is_regular_file(path) != 1)
  {
    return error_response(socket_fd, req, "403 Forbidden", server_name);
  }

  // Si algun parametro es invalido
  if (!req || !path || !body || !server_name || socket_fd < 0)
  {
    fprintf(stderr, "ERROR en la entrada de procesa_post\n");
    return error_response(socket_fd, req, "500 Internal Server Error", server_name);
  }

  char buffer[TAM_BUFFER] = {0}, response[TAM_BUFFER] = {0};

  long int out_size;
  char * script_output;

  // Si no esta soportado el lenguaje o no es un script  
  if (strcmp(ext, ".py") != 0 && strcmp(ext, ".php") != 0)
  {
    return error_response(socket_fd, req, "405 Method Not Allowed", server_name);
  }

  // Si no existe el script
  if (file_exists(path) != 0)
  {
    return error_response(socket_fd, req, "404 Not Found", server_name);
  }

  // Correr script
  char *lang = strcmp(ext, ".py") == 0 ? "python3" : "php";

  script_output = run_script(lang, path, body);
  out_size = strlen(script_output);

  // Montaje de la response
  sprintf(response, "HTTP/1.%d 200 OK\r\n", req->minor_version);
  strcat(buffer, response);

  char *now = get_time(NULL);
  sprintf(response, "Date: %s\r\n", now);
  free(now);

  strcat(buffer, response);
  sprintf(response, "Server: %s\r\n", server_name);
  strcat(buffer, response);

  char *modified_time = get_time(path);
  sprintf(response, "Last-Modified: %s\r\n", modified_time);
  free(modified_time);

  strcat(buffer, response);
  sprintf(response, "Content-Length: %ld\r\n", out_size);
  strcat(buffer, response);
  sprintf(response, "Content-Type: text/plain\r\n");
  strcat(buffer, response);
  sprintf(response, "Connection: Keep-Alive\r\n");
  strcat(buffer, response);
  sprintf(response, "Keep-Alive: timeout=5, max=20\r\n");
  strcat(buffer, response);
  sprintf(response, "\r\n");
  strcat(buffer, response);

  // Enviar los encabezados HTTP
  send(socket_fd, buffer, strlen(buffer), 0);

  // Enviar output
  send(socket_fd, script_output, out_size, 0);
  free(script_output);

  return 0;
}

/*esta funcion se ocupa de generar una response para options*/
int procesa_options(int socket_fd, PeticionHTTP *req, char *path, char *server_name, char *real_path)
{
  char buffer[TAM_BUFFER] = {0}, response[TAM_BUFFER] = {0};
  char *now = get_time(NULL);
  char *allowed;

  if (!req || !path || !server_name || socket_fd < 0 || !real_path)
  {
    fprintf(stderr, "ERROR en la entrada de procesa_post\n");
    return error_response(socket_fd, req, "405 Method Not Allowed", server_name);
  }
  if (file_exists(path) != 0)
  {
    return error_response(socket_fd, req, "404 Not Found", server_name);
  }

  // Si es script
  if (strstr(path, "/scripts/") != NULL)
  {
    allowed = "GET, POST, OPTIONS";
  }
  else if (strcmp(real_path, "/") == 0)
  {
    allowed = "GET, POST, OPTIONS";
  }
  else
  {
    allowed = "GET, OPTIONS";
  }

  // montaje de la response
  sprintf(response, "HTTP/1.%d 200 OK\r\n", req->minor_version);
  strcat(buffer, response);
  sprintf(response, "Date: %s\r\n", now);
  strcat(buffer, response);
  sprintf(response, "Server: %s\r\n", server_name);
  strcat(buffer, response);
  sprintf(response, "Allow: %s\r\n", allowed);
  strcat(buffer, response);
  sprintf(response, "Content-Length: 0\r\n");
  strcat(buffer, response);
  sprintf(response, "Connection: Close\r\n");
  strcat(buffer, response);
  sprintf(response, "\r\n");
  strcat(buffer, response);
  // envio,free de now y retorno del output
  send(socket_fd, buffer, strlen(buffer), 0);

  free(now);
  return 0;
}

/*Genera el codigo de error necesario para cada caso y lo manda como response*/
int error_response(int socket_fd, PeticionHTTP *req, char *error, char *server_name)
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

  char *now = get_time(NULL);
  sprintf(response, "Date: %s\r\n", now);
  free(now);

  strcat(buffer, response);
  sprintf(response, "Server: %s\r\n", server_name);
  strcat(buffer, response);
  sprintf(response, "Content-Length:%ld\r\n", strlen(content));
  strcat(buffer, response);

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