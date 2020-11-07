#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "server.h"
#include "client.h"
#include "daemonize.h"

int main(int argc, char *argv[]) {
    openlog(NULL, LOG_PID | LOG_CONS | LOG_PERROR, LOG_DAEMON);

    int client_fd = connect_to_daemon();
    bool daemon_running = client_fd != ERROR;

    if(daemon_running) {
        const char *command = parse_commandline(argc, argv);

        if(!send_command(client_fd, command)) {
            syslog(LOG_ERR, "Could not send command <%s> to server", command);
            exit(EXIT_FAILURE);
        }

        if(!command_acknowledged(client_fd)) {
            syslog(LOG_NOTICE, "Daemon executed command %s", command);
        } else {
            syslog(LOG_ERR, "Daemon did not execute command %s", command);
        }
    } else {
        syslog(LOG_NOTICE, "Attempting to Daemonize");

        if(!daemonize()) {
            syslog(LOG_EMERG, "Unable to Daemonize: %m");
            exit(EXIT_FAILURE);
        } 
        
        syslog(LOG_NOTICE, "Daemonization Successful");

        if(!start_server()) {
            syslog(LOG_EMERG, "Unable to spin up server: %m");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
