#include "server/CEServer.h"
#include <stdio.h>
#include <stdlib.h>

#define PORT 8080


int main(int argc, char const *argv[])
{
    printf("Initializing Server at %d", PORT);
    CEServerStr serverStr =
        {
            port_number : PORT
        };
    start_micro_http_server(serverStr);
    return 0;
}
