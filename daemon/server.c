#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>

#include "audit.h"
#include "server.h"

#define ERROR -1
#define PORT 5000
#define BACKLOG 32
#define BUFFER 16

bool fork_execute(const char *command);

bool start_server() {
    // make server socket
    int serv_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(serv_fd == ERROR) return false;

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(PORT)
    };

    // bind to the socket
    if(bind(serv_fd, (struct sockaddr*) &server_address, sizeof(server_address)) == ERROR) return false;

    // put server into listening state
    if(listen(serv_fd, BACKLOG) == ERROR) return false;

    int client_size = sizeof(struct sockaddr_in);
    struct sockaddr_in client_address;
    char command[BUFFER] = "";

    // server accepts client and receives requests
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

        // fork and execute command sent to server, then send appropriate response
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

    if(pid == 0) {
        char *from = NULL;
        char *to = NULL;
        /*
            set from and to variables to appropriate paths
            based on command sent from client
        */
        if(strcmp(command, "backup") == 0) {
            from = "/var/www/html/livesite";
            to = "/var/www/html/backup";
        } else if(strcmp(command, "transfer") == 0) {
            from = "/var/www/html/intranet";
            to = "/var/www/html/livesite";
        } else if(strcmp(command, "audit") == 0) {
            generate_report();
            syslog(LOG_INFO, "Report Generated");
        } else {
            syslog(LOG_ERR, "Unknown command %s", command);
            return false;
        }

        // program to be run to facilate moving of data
        char *program = "/home/jonny/Systems-Software-CA1/backup/backup";
        char *argv[] = {program, from, to, NULL};

        // execute program
        if(execvp(program, argv) == ERROR) {
            syslog(LOG_ERR, "Server child process failed to execute <%s> || %m || %s", command, program);
            exit(EXIT_FAILURE);
        }

    }

    syslog(LOG_INFO, "Server executed command: <%s>", command);

    return true;
}