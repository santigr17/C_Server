#include "server/CEServer.h"
#include <stdio.h>
// #include <stdlib.h>

#define PORT 8080


int main(int argc, char const *argv[])
{
    printf("Initializing Server at %d", PORT);
    CEServerStr serverStr =
        {
            port_number : PORT
        };
    struct MHD_Daemon *daemon;
    int run = start_micro_http_server(serverStr, daemon);
    // FILE *config;
    while (run <100)
    {
        run ++;
        sleep(100);
    }
    int stop = stop_micro_http_server(daemon);
    
    return 0;
}
