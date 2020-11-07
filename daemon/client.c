#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"

#define ERROR -1
#define PORT 5000
#define BUFFER 32

int connect_to_daemon() {
    // set client socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(client_fd == ERROR) return ERROR;

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_port = htons(PORT)
    };

    // connect to server
    if(connect(client_fd, (struct sockaddr*) &server_address, sizeof(server_address)) == ERROR)
        return ERROR;

    return client_fd;
}

bool send_command(int send_fd, const char *command) {
    return send(send_fd, command, strlen(command), 0) != ERROR;
}

bool command_acknowledged(int send_fd) {
    char response[BUFFER] = "";

    // receive acknowldgement
    if(recv(send_fd, response, BUFFER, 0) == ERROR) {
        syslog(LOG_ERR, "Did not recieve ACK from server");
        exit(EXIT_FAILURE);
    }

    return strcmp(response, "ack") == 0;
}

char* parse_commandline(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "usage: %s <backup> <transfer>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return argv[1];
}