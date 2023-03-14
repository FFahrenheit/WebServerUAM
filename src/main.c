#include <stdio.h>
#include "../includes/web_server.h"

/**
 * @brief funcion main que comienza los procesos para el servidor
 * 
 * @return int 0 exito, bien cerrado
 */
int main()
{
    //crea el servidor
    struct WebServer server= create_server();
    //launch del servidor
    server.launch(&server);
    return 0;
}
