#include <stdio.h>
#include "include/WebServer.h"

int main() {
    printf("Hello, World!\n");
    struct WebServer server = create_server(8080, 100);
    server.server.launch(&server.server);

    
    return 0;
}
