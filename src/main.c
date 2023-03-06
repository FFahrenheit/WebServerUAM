#include <stdio.h>
#include "../includes/web_server.h"


int main() {

    struct WebServer server = create_server();
    server.launch(&server);
    return 0;
}
