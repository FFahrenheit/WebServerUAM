//
// Created by ivan on 2/12/23.
//

#include "../includes/web_server.h"

#define POOL_LENGTH 10

pthread_mutex_t mutex;
int socket1, socket2;

/**
 * @brief Funcion que maneja la senal de ctrl + c
 * 
 * @param sig ID de la senal recibida
 */
void handler_SIGINT(int sig)
{
    close_socket(socket1);
    close_socket(socket2);

    printf("Sockets cerrados\n");
    exit(0);
}

/**
 * @brief Funcion que inicia a correr el servidor http, crea los threads en cuestion
 * e inicializa la lectura
 * 
 * @param webServer estructura del servidor en cuestion
 */
void run_server(struct WebServer *webServer)
{
    struct sigaction act1;
    act1.sa_handler = handler_SIGINT;
    sigemptyset(&(act1.sa_mask));
    sigaddset(&(act1.sa_mask), SIGINT);
    act1.sa_flags = 0;

    if (sigaction(SIGINT, &act1, NULL) < 0)
    {
        perror("sigint");
        exit(EXIT_FAILURE);
    }

    printf("\n\n\n╔═══╗╔╗────────╔╗─╔╗──╔╗─────\n║╔═╗╠╝╚╗───────║║─║║──║║─────\n║╚══╬╗╔╬══╦╗─╔╗║║─║╠══╣║╔╗─╔╗\n╚══╗║║║║╔╗║║─║║║║─║║╔╗║║║║─║║\n║╚═╝║║╚╣╔╗║╚═╝║║╚═╝║╚╝║╚╣╚═╝║\n╚═══╝╚═╩╝╚╩═╗╔╝╚═══╩═╗╠═╩═╗╔╝\n──────────╔═╝║─────╔═╝║─╔═╝║─\n──────────╚══╝─────╚══╝─╚══╝─\n\n\n\n");
    printf("Server built!\n");
    fflush(stdout);

    /* Iterativo
    struct Server * server = &webServer->server;
    int address_length = sizeof(server->address);
    int new_socket;

    struct sigaction act1;

    //Iterativo
    socket1 = 0;
    socket2 = 0;

    act1.sa_handler = handler_SIGINT;
    sigemptyset(&(act1.sa_mask));
    sigaddset(&(act1.sa_mask), SIGINT);
    act1.sa_flags = 0;

    if (sigaction(SIGINT, &act1, NULL) < 0) {
        perror("sigint");
        exit(EXIT_FAILURE);
    }
    sigemptyset(&(act1.sa_mask));
    sigaddset(&(act1.sa_mask), SIGINT);
    act1.sa_flags = 0;

    if (sigaction(SIGINT, &act1, NULL) < 0) {
        perror("sigint");
        exit(EXIT_FAILURE);
    }


    while(!0)
    {
        printf("----- ESPERANDO CONEXION -----\n");

        new_socket = accept(server->socket,
                            (struct sockaddr *) &server->address,
                            (socklen_t *) &address_length);

        socket1 = server->socket;
        socket2 = new_socket;


        procesa_cliente(new_socket, webServer->server_signature, webServer->server_root);


        close(new_socket);
        printf("Conexion cerrada\n");
    }
*/
    // Reactivo
    /*
    while(!0)
    {
        int childpid;
        printf("----- ESPERANDO CONEXION -----\n");

        new_socket = accept(server->socket,
                            (struct sockaddr *) &server->address,
                            (socklen_t *) &address_length);

        if( (childpid = fork()) == 0)
        {
            printf("fork creado\n");
            child_handle_fork(new_socket);
            printf("fork finalizado");
        }
    }
     */

    // Pool

    int childpid;
    // Creamos mutex para manejar la pool
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        perror("No se pudo crear el mutex");
        exit(1);
    }

    for (int i = 0; i < POOL_LENGTH; i++)
    {
        if ((childpid = fork()) == 0)
        {
            child_handle(i, webServer);
        }
    }

    while (!!1){};
}

struct WebServer create_server()
{
    // Se lee el archivo de configuracion
    struct ServerConfig config = read_config("server.conf");

    // Se crea el servidor con los correspondientes parametros de un servidor HTTP
    struct Server server = server_constructor(
        AF_INET,
        SOCK_STREAM,
        0,
        INADDR_ANY,
        config.port,
        config.max_clients);

    struct WebServer web_server;
    web_server.server = server;
    web_server.launch = run_server;
    web_server.server_root = config.root;
    web_server.server_signature = config.name;

    // printf("PORT:%d|\nBACKLOG:%d|\nNAME:%s|\nROOT:%s|\n", config.port, config.max_clients, web_server.server_signature, web_server.server_root);

    return web_server;
}

/*
void child_handle_fork(int new_socket)
{
    char buffer[30000];
    char * hello = "Hello!";
    read(new_socket, buffer, 30000);
    //printf("%s\n", buffer);

    write(new_socket, hello, strlen(hello));
    close(new_socket);
}

*/
void child_handle(int pool_id, struct WebServer *webServer)
{
    int address_length = sizeof(webServer->server.address);
    int new_socket;

    while (1)
    {
        printf("----- ESPERANDO CONEXION %i -----\n", pool_id);

        // Se usa un lock para proteger el acceso al recurso compartido de socket
        pthread_mutex_lock(&mutex);
        new_socket = accept(webServer->server.socket,
                            (struct sockaddr *)&webServer->server.address,
                            (socklen_t *)&address_length);
        pthread_mutex_unlock(&mutex);
        
        // Se procesa el cliente en el proceso fork
        procesa_cliente(new_socket, webServer->server_signature, webServer->server_root);

        // Se cierra el socket. Cabe mencionar que esto sucede cuando el cliente decide cerrarlo o no han habido peticiones
        close(new_socket);
    }
}