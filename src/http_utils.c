#include "../includes/http_utils.h"

#define TAM_STR 200      // Longitud de cadenas leidas
#define TAM_BUFFER 30000 // Longitud del buffer leido

/**
 * @brief Obtiene el ultimo tiempo de modificacion de un recurso, o el tiempo actual
 * en formato de servidor
 *
 * @param path Ruta al recurso en cuestion, si es nulo es el tiempo actual
 * @return char* Cadena con el tiempo solicitado en formato servidor
 */
char *get_time(char *path)
{
    char *response;
    struct tm tm;

    // Si el path es nulo, obtenemos el tiempo actual
    if (path == NULL)
    {
        time_t now = time(0);
        tm = *gmtime(&now);
    }
    else
    {
        struct stat attr;

        // Leemos atributos del archivo
        if (stat(path, &attr) != 0)
        {
            time_t now = time(0);
            tm = *gmtime(&now);
        }
        else // No se pudo obtener el tiempo, se usa el actual
        {
            time_t mtime = attr.st_mtime;
            tm = *gmtime(&mtime);
        }
    }

    char buf[1000];
    // Formato de tiempo del servidor
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    response = malloc(strlen(buf) + 1);

    strcpy(response, buf);
    // printf("Time for %s: [%s]\n", (path == NULL) ? "now" : path, buf);

    return response;
}

/**
 * @brief Genera una subcadena a partir de una cadena original, su indice de inicio (inclusivo)
 * y fin (no inclusivo)
 *
 * @param str Cadena original
 * @param start Indice de inicio
 * @param end Indice final
 * @return char* Subcadena
 */
char *substring(const char *str, int start, int end)
{
    int len = end - start;
    char *sub = malloc(len + 1);
    strncpy(sub, str + start, len);
    sub[len] = '\0';
    return sub;
}

/**
 * @brief A partir de una cadena del formato <clave> = <valor> asigna en los punteros la clave y valor
 * en cuestion (usado para leer el archivo de configuracion)
 *
 * @param input Cadena original
 * @param left Puntero a la clave
 * @param right Puntero al valor
 * @return int 0 exito, -1 error
 */
int read_key(char *input, char **left, char **right)
{
    int len = strlen(input);
    char *key, *value;

    // Leemos la cadena hasta encontrar los caracteres ' = '
    for (int i = 0; i < len - 2; i++)
    {
        if (input[i] == ' ' && input[i + 1] == '=' && input[i + 2] == ' ')
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

/**
 * @brief Lee la configuracion del archivo especificado y lo retorna en una struct ServerConfig
 *
 * @param filename nombre del archivo de donde se lee la configuracion
 * @return struct ServerConfig Donde se almacenan las configuraciones leidas
 */
struct ServerConfig read_config(char *filename)
{
    struct ServerConfig config;
    FILE *fp_conf;
    char *key = {0}, *value = {0};
    char *server_name, *server_root;
    char buffer[TAM_STR];
    int port = 0, backlog = 0;

    fp_conf = fopen(filename, "r");

    while (fp_conf && fgets(buffer, TAM_STR, fp_conf))
    {
        read_key(buffer, &key, &value);
        if (key == NULL)
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
            server_name = malloc(strlen(value) + 1);
            strcpy(server_name, value);
        }
        else if (strcmp("server_root", key) == 0)
        {
            server_root = malloc(strlen(value) + 1);
            strcpy(server_root, value);
        }

        free(key);
        free(value);
    }
    
    if(fp_conf)
    {
        fclose(fp_conf);
    }

    // Validar valores leidos
    if (backlog == 0)
    {
        printf("WARNING: Using default backlog: 10\n");
        backlog = 10;
    }
    if (port == 0)
    {
        printf("WARNING: Using default port: 8000\n");
        port = 8000;
    }
    if (!server_name)
    {
        printf("WARNING: Using default server name: Redes II Server\n");
        server_name = "Redes II Server";
    }
    if (!server_root)
    {
        printf("WARNING: Using default server root: wwww\n");
        server_root = "www";
    }

    config.max_clients = backlog;
    config.name = server_name;
    config.port = port;
    config.root = server_root;

    return config;
}

/**
 * @brief Verifica si un archivo existe
 *
 * @param path Ruta al archivo
 * @return int 0: Existe, -1: No existe
 */
int file_exists(char *path)
{
    return (access(path, F_OK) == 0) ? 0 : -1;
}

/**
 * @brief Corre el script del path en el lenguaje adecuado con los argumentos correspondientes y
 * retorna su salida (hasta fin y \r\n)
 *
 * @param lang Interprete a utilizar (python, php)
 * @param path Ruta al script
 * @param arguments Argumentos en formato peticion
 * @return char* Salida
 */
char *run_script(char *lang, char *path, char *arguments)
{
    FILE *script;
    char command[TAM_BUFFER] = {0}, result[TAM_BUFFER] = {0};
    char output[TAM_STR] = {0};
    sprintf(command, "%s %s '%s'", lang, path, (arguments == NULL) ? "" : arguments);

    // printf(">RUN: %s\n", command);

    // Open a pipe to the script with arguments
    script = popen(command, "r");

    // Leer la salida del script
    while (fgets(output, sizeof(output), script))
    {
        int length = strlen(output);
        strcat(result, output);

        // Hasta \r\n
        if (output != NULL && strlen(output) >= 2 &&
            output[length - 2] == '\r' && output[length - 1] == '\n')
        {
            break;
        }
    }
    // Cerrar pipe
    pclose(script);

    // Guardar la salida en un puntero con malloc
    char *return_output = malloc(strlen(result) + 1);
    strcpy(return_output, result);

    // No olvidar liberar la memoria de la salida
    return return_output;
}

/**
 * @brief Verifica si un recurso es un archivo o una carpeta o directorio
 *
 * @param path Ruta al recurso
 * @return int 1 Si es archivo normal
 */
int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}