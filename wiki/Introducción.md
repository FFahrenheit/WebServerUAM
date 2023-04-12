En esta práctica se ha desarrollado un servidor HTTP básico que permite servir archivos estáticos a través de la web. El funcionamiento de este servidor se basa en el protocolo HTTP, que es el estándar utilizado para la transferencia de información en la web.

El servidor HTTP se encarga de escuchar las peticiones realizadas por los clientes (por ejemplo, un navegador web), procesarlas y responder con la información solicitada. Para ello, el servidor utiliza una serie de algoritmos y técnicas que se han implementado en el código, como la gestión de sockets, la interpretación de cabeceras HTTP para el manejo de las respuestas y la configuración del servidor a través de ficheros auxiliares

Uno de los objetivos principales de esta práctica ha sido aprender a utilizar técnicas para desarrollar un proyecto de cierta complejidad, como es un servidor web. Se ha utilizado una metodología de desarrollo escalable implementando pieza a pieza cada parte del servidor por separado para así ir reduciendo en abstracción hasta lo más tangible.Todo esto se desarrollará con más ímpetu en los siguientes apartados de la memoria.

## Como correr el servidor
Para ejecutar el programa, simplemente clone el repositorio en cuestión y agregue los recursos del servidor que pueden ser accedidos desde [este enlace](https://git.eps.uam.es/redes2/2023/2311/01/practica1/-/wikis/Recursos-para-servidor). 

Asimismo, recuerde configurar los detalles del servidor en `server.conf`

Para compilar:

`make all`


Corra el archivo ejecutable

`./main`