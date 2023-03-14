//
// Created by ivan on 2/12/23.
//

#include "../includes/server.h"


/*constructor del servodpr*/
struct Server server_constructor(int domain, int service, int protocol, unsigned long int interface, int port, int backlog)
{
    if(!domain||!service||protocol<0||interface||port<=0||backlog<1)
    {
        perror("error en los argumentos para la creacion y construccion del server");
        exit(-1);
    }
    struct Server server;

    // Asignamos las propiedades del servidor
    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);             // Convertir de int a orden de red TCP/IP (big-endian)
    server.address.sin_addr.s_addr = htonl(interface); // Convertir de long a orden de red TCP/IP

    // Creamos el Socket para aceptar conexiones
    server.socket = open_socket(domain, service, protocol);

    if ((bind_socket(server.socket,
                     (struct sockaddr *)&server.address, // Cast a socketaddr *
                     sizeof(server.address))) < 0)
    {
        perror("No se pudo hacer bind correctamente");
        exit(1);
    }

    // Escuchamos en el socket hasta backlog clientes
    if ((listen(server.socket, server.backlog)) < 0)
    {
        perror("No se pudo escuchar");
        exit(1);
    }
    return server;
}
/*proceso de binding del socket*/
int bind_socket(int socket, struct sockaddr *address, int address_len)
{
    //funcion para el bind y retorno del output de esta
    return bind(socket, address, address_len);
}

/*funcion que se ocurra de abrir el socket*/
int open_socket(int domain, int service, int protocol)
{

    int socket_fd;
    /*creacion del socket*/
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    //control de errores para la creacion del socket
    if (socket_fd < 0)
    {
        fprintf(stderr, "ERROR, al abrir server");
        exit(1);
    }

    if (socket_fd == 0)
    {
        perror("No se pudo conectar el socket");
        exit(1);
    }

    // Hacer que el sistema operativo reutilice la direccion para evitar TIME_WAIT
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
        perror("No se pudo reutilzar address");
        exit(1);
    }
    //retorno del descriptor del socket
    return socket_fd;
}

/*funcion que se ocupa a cerrar el socket*/
void close_socket(int socket_fd)
{
    //confirmamos la existencia del socket antes de cerrarlo
    if(socket_fd <0)
    {
        close(socket_fd);
    }
    return;
}
