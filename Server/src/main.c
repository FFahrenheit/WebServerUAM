#include <stdio.h>
#include "web_server.h"


int main() {
    printf("Hello, World!\n");
    struct WebServer server = create_server(8080, 100);
    server.launch(&server);
    return 0;
}
