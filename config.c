#include "config.h"


 int get_port_config()
{
    int temp_port = DEFAULT_PORT;
    config_init(&cfg);
    if(config_read_file(&cfg, FILE_CONFIG) == CONFIG_TRUE)
    {
        port_server_setting = config_lookup(&cfg, "server.port");
        temp_port = config_setting_get_int(port_server_setting);
    }
    else
        printf("ERROR: No se pudo leer el archivo de configuracion durante 'get_port_config'");

    config_destroy(&cfg);
    return temp_port;
}

 int get_sleep_time()
{
    int temp_time = DEFAULT_PORT;
    config_init(&cfg);
    if(config_read_file(&cfg, FILE_CONFIG) == CONFIG_TRUE)
    {
        sleep_daemon_setting = config_lookup(&cfg, "daemon.sleep");
        temp_time = config_setting_get_int(sleep_daemon_setting);
    }
    else
        printf("ERROR: No se pudo leer el archivo de configuracion durante 'get_sleep_time'");
    
    config_destroy(&cfg);
    return temp_time;
}

int check_active()
{
    // NO LO DETINE
    int temp_active = 1;
    config_init(&cfg);
    if(config_read_file(&cfg, FILE_CONFIG) == CONFIG_TRUE)
    {
        active_server_setting = config_lookup(&cfg, "server.active");
        temp_active = config_setting_get_int(active_server_setting);
    }
    else
        printf("ERROR: No se pudo leer el archivo de configuracion durante 'get_sleep_time'");
    
    config_destroy(&cfg);
    return temp_active;
}

 void update_active(int value)
{
    config_init(&cfg);
    if(config_read_file(&cfg, FILE_CONFIG) == CONFIG_TRUE)
    {
        active_server_setting = config_lookup(&cfg, "server.active");
        config_setting_set_int(active_server_setting, 1);
        config_write_file(&cfg, FILE_CONFIG);
    }
    else
        printf("ERROR: No se pudo leer el archivo de configuracion durante 'get_sleep_time'");
    
    config_destroy(&cfg);
}

