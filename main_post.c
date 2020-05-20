#include "server/CEServer.h"
#include <stdio.h>
#include <stdlib.h>
#include "config.h"

int main(int argc, char const *argv[])
{
    // CONSTANTES CONFIGURABLES
    int PORT, SLEEP_TIME, ACTIVE;
    // GETTING CONFIG
    PORT = get_port_config();
    printf("Puerto leido: %d\n", PORT);
    SLEEP_TIME = get_sleep_time();
    printf("TIEMPO LEIDO: %d\n", SLEEP_TIME);
    
    
    // STARTING SERVER
    printf("Iniciando el Servidor Image\n");
    struct MHD_Daemon *daemon;
    ACTIVE = start_micro_http_server(PORT, daemon);
    printf("ACTIVE obtenido: %d\n", ACTIVE);
    
    if(!ACTIVE){
        printf("ERROR: No se pudo inicializar el servidor\n");
        return 1;
    }
    printf("Image Server activo en el Puerto: %d\n", 902);
    update_active(ACTIVE);        
    while (ACTIVE)
    {
        printf("Server activo\n");
        ACTIVE = check_active();
        sleep(SLEEP_TIME);
        
    }
    printf("Terminando el Image Server ...");
    int stop = stop_micro_http_server(daemon);
    printf("Finalizado.\n");
    return 0;
}
