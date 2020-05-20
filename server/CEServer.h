#ifndef CEServer_HEADER_FILE
#define CEServer_HEADER_FILE
#include <microhttpd.h>

/**
 * This is an abstraction of the CE server which is going to be the server block
 */
typedef struct
{
    /**
     * The port number
     */
    int port_number;
} CEServerStr;
/**
 * Starts a server
 */ 
int start_server(CEServerStr serverStr);
/**
 * Starts an  server
 */ 
int start_http_server(CEServerStr serverStr);

/**
 * 
 */ 
int answer_to_connection (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls);

/**
 * 
 */ 
int start_micro_http_server (CEServerStr serverStr);

#endif
