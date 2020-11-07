#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include "server.h"

#define ERROR -1
#define PORT 5000
#define BACKLOG 32
#define BUFFER 16

bool fork_execute(const char *command);

bool start_server() {
    int serv_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(serv_fd == ERROR) return false;

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(PORT)
    };

    if(bind(serv_fd, (struct sockaddr*) &server_address, sizeof(server_address)) == ERROR) return false;

    if(listen(serv_fd, BACKLOG) == ERROR) return false;

    int client_size = sizeof(struct sockaddr_in);
    struct sockaddr_in client_address;
    char command[BUFFER] = "";

    while(1) {
        int client_fd = accept(serv_fd, (struct sockaddr*) &client_address, (socklen_t *) &client_size);

        if(client_fd == ERROR) {
            syslog(LOG_ERR, "Server unable to accept incoming connection");
            continue;
        }
            
        if(recv(client_fd, command, BUFFER, 0) == ERROR) {
            syslog(LOG_ERR, "Server unable to recieve incoming request");
            continue;
        }

        syslog(LOG_ERR, "Command %s recieved by server", command);

        if(fork_execute(command)) {
            write(client_fd, "ack", strlen("ack"));
        } else {
            write(client_fd, "nack", strlen("nack"));
        }
    }
}

bool fork_execute(const char *command) {
    pid_t pid = fork();

    if(pid == ERROR) {
        syslog(LOG_ERR, "Server unable to execute %s", command);
        return false;
    }

    // change paths later
    if(pid == 0) {
        char *argv[] = {"/home/jonny/labs/assignment/backup/backup", NULL};
        char *absolute_path = "/home/jonny/labs/assignment/backup/backup";

        if(execvp(absolute_path, argv) == ERROR) {
            syslog(LOG_ERR, "Server child process failed to execute <%s> || %m || %s", command, absolute_path);
            exit(EXIT_FAILURE);
        }
    }

    syslog(LOG_INFO, "Server executed command: <%s>", command);

    return true;
}