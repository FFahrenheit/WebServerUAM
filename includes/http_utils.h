#ifndef HTTP_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

/**
 * @brief Estructura encargada de guardar los diferentes parametros de configuracion
 * en el config file
 * 
 */
struct ServerConfig
{
  int port;         // Puerto en que corre el servidor
  int max_clients;  // Maximos clientes a esperar en cola
  char *root;       // Ubicacion raiz de los recursos del servidor
  char *name;       // Nombre del servidor
};

// Declaraciones para el mapeo de extensiones
typedef struct
{
  char *extension;  // Extension del recurso
  char *etiqueta;   // MIME Type
} Extension;

/**
 * @brief Obtiene el ultimo tiempo de modificacion de un recurso, o el tiempo actual
 * en formato de servidor
 *
 * @param path Ruta al recurso en cuestion, si es nulo es el tiempo actual
 * @return char* Cadena con el tiempo solicitado en formato servidor
 */
char *get_time(char *path);

/**
 * @brief Genera una subcadena a partir de una cadena original, su indice de inicio (inclusivo) 
 * y fin (no inclusivo) 
 *
 * @param str Cadena original
 * @param start Indice de inicio
 * @param end Indice final
 * @return char* Subcadena
 */
char *substring(const char *str, int start, int end);

/**
 * @brief A partir de una cadena del formato <clave> = <valor> asigna en los punteros la clave y valor
 * en cuestion (usado para leer el archivo de configuracion)
 *
 * @param input Cadena original
 * @param left Puntero a la clave 
 * @param right Puntero al valor
 * @return int 0 exito, -1 error
 */
int read_key(char *input, char **left, char **right);

/**
 * @brief Lee la configuracion del archivo especificado y lo retorna en una struct ServerConfig
 * 
 * @param filename nombre del archivo de donde se lee la configuracion 
 * @return struct ServerConfig Donde se almacenan las configuraciones leidas
 */
struct ServerConfig read_config(char *filename);

/**
 * @brief Verifica si un archivo existe
 *
 * @param path Ruta al archivo
 * @return int 0: Existe, -1: No existe
 */
int file_exists(char *path);

/**
 * @brief Corre el script del path en el lenguaje adecuado con los argumentos correspondientes y
 * retorna su salida (hasta fin y \r\n)
 * 
 * @param lang Interprete a utilizar (python, php)
 * @param path Ruta al script
 * @param arguments Argumentos en formato peticion
 * @return char* Salida
 */
char *run_script(char *lang, char *path, char *arguments);

/**
 * @brief Verifica si un recurso es un archivo o una carpeta o directorio
 *
 * @param path Ruta al recurso
 * @return int 1 Si es archivo normal 
 */
int is_regular_file(const char *path);

#define HTTP_UTILS_H
#endif // HTTP_UTILS_H
