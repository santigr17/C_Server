
#include "CEServer.h"

static const char LOG_FILE[] = "/var/log/ce-image-server.log";

/**
 * Agregado para el POST
 */

// HTMLs prueba
const char *greatingpage = "<html><body><h1>Welcome, %s!</center></h1></body></html>";
const char *errorpage = "<html><body>This doesn't seem to be right.</body></html>";
const char *busypage = "<html><body>This server is busy, please try again later.</body></html>";
const char *askpage = "<html><body>\n\
                       Upload a file, please!<br>\n\
                       There are %u clients uploading at the moment.<br>\n\
                       <form action=\"/filepost\" method=\"post\" \
                         enctype=\"multipart/form-data\">\n\
                       <input name=\"file\" type=\"file\">\n\
                       <input type=\"submit\" value=\" Send \"></form>\n\
                       </body></html>";
const char *completepage = "<html><body>The upload has been completed.</body></html>";
const char *servererrorpage = "<html><body>An internal server error has occured.</body></html>";
const char *fileexistspage = "<html><body>This file already exists.</body></html>";
const char *fileioerror = "<html><body>IO error writing to disk.</body></html>";
const char *const postprocerror = "html><head><title>Error</title></head><body>Error processing POST data</body></html>";

/**
 * INSTANCIA PRINCIPAL DEL SERVIDOR
 */ 

static int
send_page(struct MHD_Connection *connection,
          const char *page, int status_code)
{
  int ret;
  struct MHD_Response *response;

  response = MHD_create_response_from_buffer(strlen(page), (void *)page,
                                             MHD_RESPMEM_MUST_COPY);
  if (!response)
    return MHD_NO;

  ret = MHD_queue_response(connection, status_code, response);
  MHD_destroy_response(response);

  return ret;
}
static int
iterate_post (void *coninfo_cls,
              enum MHD_ValueKind kind,
              const char *key,
              const char *filename,
              const char *content_type,
              const char *transfer_encoding,
              const char *data,
              uint64_t off,
              size_t size)
{
  struct connection_info_struct *con_info = coninfo_cls;
  FILE *fp;
  (void)kind;               /* Unused. Silent compiler warning. */
  (void)content_type;       /* Unused. Silent compiler warning. */
  (void)transfer_encoding;  /* Unused. Silent compiler warning. */
  (void)off;                /* Unused. Silent compiler warning. */

  if (0 != strcmp (key, "file"))
    {
      con_info->answerstring = servererrorpage;
      con_info->answercode = MHD_HTTP_BAD_REQUEST;
      return MHD_YES;
    }

  if (! con_info->fp)
    {
      if (0 != con_info->answercode) /* something went wrong */
        return MHD_YES;
      if (NULL != (fp = fopen (filename, "rb")))
        {
          fclose (fp);
          con_info->answerstring = fileexistspage;
          con_info->answercode = MHD_HTTP_FORBIDDEN;
          return MHD_YES;
        }
      /* NOTE: This is technically a race with the 'fopen()' above,
         but there is no easy fix, short of moving to open(O_EXCL)
         instead of using fopen(). For the example, we do not care. */
      con_info->fp = fopen (filename, "ab");
      if (!con_info->fp)
        {
          con_info->answerstring = fileioerror;
          con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
          return MHD_YES;
        }
    }

  if (size > 0)
    {
      if (! fwrite (data, sizeof (char), size, con_info->fp))
        {
          con_info->answerstring = fileioerror;
          con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
          return MHD_YES;
        }
    }

  return MHD_YES;
}

static void
request_completed (void *cls,
                   struct MHD_Connection *connection,
                   void **con_cls,
                   enum MHD_RequestTerminationCode toe)
{
  struct connection_info_struct *con_info = *con_cls;
  (void)cls;         /* Unused. Silent compiler warning. */
  (void)connection;  /* Unused. Silent compiler warning. */
  (void)toe;         /* Unused. Silent compiler warning. */

  if (NULL == con_info)
    return;

  if (con_info->connectiontype == POST)
    {
      if (NULL != con_info->postprocessor)
        {
          MHD_destroy_post_processor (con_info->postprocessor);
          nr_of_uploading_clients--;
        }

      if (con_info->fp)
        fclose (con_info->fp);
    }

  free (con_info);
  *con_cls = NULL;
}

/**
 * Hasta aqui
 */

/**
 * Starts a server
 */
// int start_server(CEServerStr serverStr)
// {
// //   char server_messages[1024] = "HTTP/1.1 200 OK\r\n"
// // "Content-Type: text/html; charset=UTF-8\r\n\r\n"
// // "<!DOCTYPE html><html><head><title>CE IMAGE Server</title>"
// // "<style>body { background-color: #FFFFF }"
// // "h1 { font-size:2cm; text-align: center; color: black;"
// // " text-shadow: 0 0 2mm red}</style></head>"
// // "<body><h1> CE IMMAGE SERVER</h1></body></html>\r\n";
//   // create the server socket

//   int server_socket;

//   server_socket = socket(AF_INET, SOCK_STREAM, 0);

//   // define the server address
//   struct sockaddr_in server_address;
//   server_address.sin_family = AF_INET;
//   server_address.sin_port = htons(serverStr.port_number);
//   server_address.sin_addr.s_addr = INADDR_ANY;

//   // bind the socket to our specified IP and port
//   bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
//   // 5 as the number of the connections
//   listen(server_socket, 5);
//   // accept the connection
//   int client_socket;
//   struct sockaddr_in client_address;
//   while (1)
//   {
//     client_socket = accept(server_socket, NULL, NULL);
//     //client_socket = accept(server_socket, (struct sockaddr *)&client_address, ((socklen_t *)sizeof(client_address)));

//     if (client_socket < 0)
//     {
//       perror("The client is not connected");
//       return 1;
//     }
//     else
//     {

//       // receive data
//       char client_request[2048];

//       recv(client_socket, &client_request, 2048,0 );

//       //print out the server's response
//       printf("The client sent the data: %s\n", client_request);

//       send(client_socket, server_messages, sizeof(server_messages), 0);
//       // printf("Server sending message");
//       close(client_socket);
//     }
//   }
//   close(server_socket);
//   return 0;
// }

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

/**
 * Simple GET 
 */
// int answer_to_connection (void *cls, struct MHD_Connection *connection,
//                           const char *url,
//                           const char *method, const char *version,
//                           const char *upload_data,
//                           size_t *upload_data_size, void **con_cls)
// {

//   const char *page  = "<html><body>Hello, browser!</body></html>";

//   struct MHD_Response *response;
//   int ret;

//   response = MHD_create_response_from_buffer (strlen (page),
//                                             (void*) page, MHD_RESPMEM_PERSISTENT);
//   ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
//   MHD_destroy_response (response);

//   return ret;
// }

/**
 * Agregado para POST
 */
static int
answer_to_connection(void *cls,
                     struct MHD_Connection *connection,
                     const char *url,
                     const char *method,
                     const char *version,
                     const char *upload_data,
                     size_t *upload_data_size,
                     void **con_cls)
{
  (void)cls;     /* Unused. Silent compiler warning. */
  (void)url;     /* Unused. Silent compiler warning. */
  (void)version; /* Unused. Silent compiler warning. */

  if (NULL == *con_cls)
  {
    /* First call, setup data structures */
    struct connection_info_struct *con_info;

    if (nr_of_uploading_clients >= MAXCLIENTS)
      return send_page(connection,
                       busypage,
                       MHD_HTTP_SERVICE_UNAVAILABLE);

    con_info = malloc(sizeof(struct connection_info_struct));
    if (NULL == con_info)
      return MHD_NO;
    con_info->answercode = 0; /* none yet */
    con_info->fp = NULL;

    if (0 == strcasecmp(method, MHD_HTTP_METHOD_POST))
    {
      con_info->postprocessor =
          MHD_create_post_processor(connection,
                                    POSTBUFFERSIZE,
                                    &iterate_post,
                                    (void *)con_info);

      if (NULL == con_info->postprocessor)
      {
        free(con_info);
        return MHD_NO;
      }

      nr_of_uploading_clients++;

      con_info->connectiontype = POST;
    }
    else
    {
      con_info->connectiontype = GET;
    }

    *con_cls = (void *)con_info;

    return MHD_YES;
  }

  if (0 == strcasecmp(method, MHD_HTTP_METHOD_GET))
  {
    /* We just return the standard form for uploads on all GET requests */
    char buffer[1024];

    snprintf(buffer,
             sizeof(buffer),
             askpage,
             nr_of_uploading_clients);
    return send_page(connection,
                     buffer,
                     MHD_HTTP_OK);
  }

  if (0 == strcasecmp(method, MHD_HTTP_METHOD_POST))
  {
    struct connection_info_struct *con_info = *con_cls;

    if (0 != *upload_data_size)
    {
      /* Upload not yet done */
      if (0 != con_info->answercode)
      {
        /* we already know the answer, skip rest of upload */
        *upload_data_size = 0;
        return MHD_YES;
      }
      if (MHD_YES !=
          MHD_post_process(con_info->postprocessor,
                           upload_data,
                           *upload_data_size))
      {
        con_info->answerstring = postprocerror;
        con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
      }
      *upload_data_size = 0;

      return MHD_YES;
    }
    /* Upload finished */
    if (NULL != con_info->fp)
    {
      fclose(con_info->fp);
      con_info->fp = NULL;
    }
    if (0 == con_info->answercode)
    {
      /* No errors encountered, declare success */
      con_info->answerstring = completepage;
      con_info->answercode = MHD_HTTP_OK;
    }
    return send_page(connection,
                     con_info->answerstring,
                     con_info->answercode);
  }

  /* Note a GET or a POST, generate error */
  return send_page(connection,
                   errorpage,
                   MHD_HTTP_BAD_REQUEST);
}

int start_micro_http_server (int PORT, struct MHD_Daemon *daemon)
{
  
  int start = 1;
  daemon = MHD_start_daemon (
      MHD_USE_INTERNAL_POLLING_THREAD,
      PORT, NULL, NULL,
      &answer_to_connection, NULL,
      MHD_OPTION_NOTIFY_COMPLETED,
      &request_completed, NULL,
      MHD_OPTION_END);

  if (NULL == daemon){
    printf("ERROR: No se pudo crear el daemon");
    return 0;
  }
  
  return start;
}

int stop_micro_http_server (struct MHD_Daemon *daemon) {
  MHD_stop_daemon(daemon);
}