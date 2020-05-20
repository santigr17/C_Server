CC = gcc
OUTPUT_FILE = ce.image.server.daemon.o
SERVICE_NAME = ce-image-server
SERVICE_LOG_FILE = ce-image-server.log
SERVICE_LOG_PATH = /var/log
SERVICE_FILE = ${SERVICE_NAME}.service
SERVICE_PATH_FILE = /etc/systemd/system


main: main_post.c
	@${CC} ./server/CEServer.c config.c main_post.c -o ${OUTPUT_FILE} -lmicrohttpd `pkg-config --libs libconfig`

clean:
	rm *.o
	