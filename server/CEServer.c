#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> // for close
#include <string.h>
#include <sys/select.h>
#include "CEServer.h"
static const char LOG_FILE[] = "/var/log/ce-image-server.log";
/**
 * Starts a server
 */
int start_server(CEServerStr serverStr)
{
  char server_messages[1024] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>CE IMAGE Server</title>"
"<style>body { background-color: #FFFFF }"
"h1 { font-size:2cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1> CE IMMAGE SERVER</h1></body></html>\r\n";
  // create the server socket
  int server_socket;

  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // define the server address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(serverStr.port_number);
  server_address.sin_addr.s_addr = INADDR_ANY;

  // bind the socket to our specified IP and port
  bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
  // 5 as the number of the connections
  listen(server_socket, 5);
  // accept the connection
  int client_socket;
  struct sockaddr_in client_address;
  while (1)
  {
    client_socket = accept(server_socket, NULL, NULL);
    //client_socket = accept(server_socket, (struct sockaddr *)&client_address, ((socklen_t *)sizeof(client_address)));

    if (client_socket < 0)
    {
      perror("The client is not connected");
      return 1;
    }
    else
    {

      // receive data
      char client_request[2048];
      
      recv(client_socket, &client_request, 2048,0 );

      //print out the server's response
      printf("The client sent the data: %s\n", client_request);
      


      send(client_socket, server_messages, sizeof(server_messages), 0);
      // printf("Server sending message");
      close(client_socket);
    }
  }
  close(server_socket);
  return 0;
}

/**
 * Starts an http server connection
 */
int start_http_server(CEServerStr serverStr)
{
  FILE *html_data;
  html_data = fopen("index.html", "r");
  char response_data[8096];
  fgets(response_data, 8096, html_data);
  char http_header[2048] = "HTTP/1.1 200 OK\r\n\n";
  strcat(http_header, response_data);

  //creates a server socket
  int server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  // server address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(serverStr.port_number);
  server_address.sin_addr.s_addr = INADDR_ANY;
  bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
  listen(server_socket, 5);
  int client_socket;
  FILE *fp;
  fp = fopen(LOG_FILE, "a");
  fprintf(fp, "Server initialize");
  fclose(fp);
  while (1)
  {
    client_socket = accept(server_socket, NULL, NULL);
    send(client_socket, http_header, sizeof(http_header), 0);
    close(client_socket);
  }
  return 0;
}




int answer_to_connection (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls)
{
  const char *page  = "<html><body>Hello, browser!</body></html>";

  struct MHD_Response *response;
  int ret;

  response = MHD_create_response_from_buffer (strlen (page),
                                            (void*) page, MHD_RESPMEM_PERSISTENT);
  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);

  return ret;
}

int start_micro_http_server (CEServerStr serverStr)
{      
  struct MHD_Daemon *daemon;

  daemon = MHD_start_daemon (MHD_USE_POLL_INTERNALLY, serverStr.port_number, NULL, NULL,
                             &answer_to_connection, NULL, MHD_OPTION_END);
  if (NULL == daemon) return 1;

    getchar ();

  MHD_stop_daemon (daemon);
  return 0;
}
