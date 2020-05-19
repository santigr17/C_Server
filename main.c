#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


//Server
#include "httpd.h"



int main(int argc, char* argv[])
{
    FILE *fp= NULL;
    pid_t process_id = 0;
    pid_t sid = 0;
    // Create child process
    process_id = fork();
    // Indication of fork() failure
    if (process_id < 0)
    {
        printf("fork failed!\n");
        // Return failure in exit status
        exit(1);
    }
    // PARENT PROCESS. Need to kill it.
    if (process_id > 0)
    {
        printf("process_id of child process %d \n", process_id);
        // return success in exit status
        exit(0);
    }
    //unmask the file mode
    umask(0);
    //set new session
    sid = setsid();
    if(sid < 0) { exit(1); } // Return failure
    
    // Change the current working directory to root.
    chdir("/home/santii/Projects/Tarea1");
    // Close stdin. stdout and stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    // Open a log file in write mode.
    fp = fopen ("Log.txt", "w+");
    while (1)
    {
        //Dont block context switches, let the process sleep for some time
        sleep(1);
        fprintf(fp, "Server Working...\n");
        fflush(fp);
        serve_forever("8000");
        // Implement and call some function that does core work for this daemon.
    }
    fclose(fp);
    return (0);
}

void route() {
  ROUTE_START()

  ROUTE_GET("/") {
    printf("HTTP/1.1 200 OK\r\n\r\n");
    printf("Hello! You are using %s", request_header("User-Agent"));
  }

  ROUTE_GET("/test") {
    printf("HTTP/1.1 200 OK\r\n\r\n");
    printf("List of request headers:\r\n\r\n");

    header_t *h = request_headers();

    while (h->name) {
      printf("%s: %s\n", h->name, h->value);
      h++;
    }
  }

  ROUTE_POST("/") {
    printf("HTTP/1.1 200 OK\r\n\r\n");
    printf("Wow, seems that you POSTed %d bytes. \r\n", payload_size);
    printf("Fetch the data using `payload` variable.");
  }

  ROUTE_END()
}
