#include "../includes/http_utils.h"

#define TAM_STR 200


char * get_time(char * path) {
  char * response;
  struct tm tm;

  if(path == NULL)
  {
    time_t now = time(0);
    tm = *gmtime(&now);
  }
  else
  {
    struct stat attr;
    
    if(stat(path, &attr) != 0)
    {
      time_t now = time(0);
      tm = *gmtime(&now);     
    }
    else //No se pudo obtener el tiempo, se usa el actual
    {
      time_t mtime = attr.st_mtime;
      tm = *gmtime(&mtime);
    }
  }

  char buf[1000];
  strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
  response = malloc(strlen(buf)+1);
  
  strcpy(response, buf);
  printf("Time for %s: [%s]\n", (path == NULL)? "now" : path, buf);
  
  return response;
}

char *substring(const char *str, int start, int end) 
{
    int len = end - start;
    char *sub = malloc(len + 1);
    strncpy(sub, str + start, len);
    sub[len] = '\0';
    return sub;
}

int read_key(char* input, char** left, char** right) {
    int len = strlen(input);
    char * key, * value;

    for(int i = 0; i < len - 2; i++)
    {
        if(input[i] == ' ' && input[i+1] == '=' && input[i+2] == ' ')
        {
            int end = (input[len - 1] == '\n') ? len - 1 : len;
            key = substring(input, 0, i);
            value = substring(input, i + 3, end);
            *left = key;
            *right = value;
            return 0;
        }
    }
    return -1;
}

struct ServerConfig read_config(char * filename)
{
    struct ServerConfig config;
    FILE * fp_conf;
    char *key = {0}, *value={0};
    char *server_name, *server_root;
    char buffer[TAM_STR];
    int port, backlog;

    fp_conf = fopen(filename,"r");

    while(fgets(buffer, TAM_STR, fp_conf))
    {
        read_key(buffer, &key, &value);
        if(key == NULL)
        {
            continue;
        }
        
        if (strcmp("max_clients", key) == 0)
        {
            backlog = atoi(value);
        }
        else if (strcmp("listen_port", key) == 0)
        {
            port = atoi(value);
        }
        else if (strcmp("server_signature", key) == 0)
        {
            server_name = malloc(strlen(value)+1);            
            strcpy(server_name, value);
        }
        else if (strcmp("server_root", key) == 0)
        {
            server_root = malloc(strlen(value)+1);
            strcpy(server_root, value);
        }

        free(key);
        free(value);
    }
    fclose(fp_conf);    

    config.max_clients = backlog;
    config.name = server_name;
    config.port = port;
    config.root = server_root;

    return config;
}

int file_exists(char * path)
{
    return (access(path, F_OK) == 0) ? 0 : -1; 
}