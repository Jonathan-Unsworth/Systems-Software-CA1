#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>

#include "server.h"
#include "client.h"
#include "daemonize.h"
#include "audit.h"

int main(int argc, char *argv[]) {
    openlog(NULL, LOG_PID | LOG_CONS | LOG_PERROR, LOG_DAEMON);

    int client_fd = connect_to_daemon();
    bool daemon_running = client_fd != ERROR;

    if(daemon_running) {
        // get command input into cmd
        const char *command = parse_commandline(argc, argv);

        // send command to server
        if(!send_command(client_fd, command)) {
            syslog(LOG_ERR, "Could not send command <%s> to server", command);
            exit(EXIT_FAILURE);
        }

        // acknowledge if command was executed
        if(!command_acknowledged(client_fd)) {
            syslog(LOG_NOTICE, "Daemon executed command %s", command);
        } else {
            syslog(LOG_ERR, "Daemon did not execute command %s", command);
        }
    } else {
        syslog(LOG_NOTICE, "Attempting to Daemonize");

        // if daemon did not set up, log it
        if(!daemonize()) {
            syslog(LOG_EMERG, "Unable to Daemonize: %m");
            exit(EXIT_FAILURE);
        } 
        
        syslog(LOG_NOTICE, "Daemonization Successful");

        // set a watch on the web directory
        if(set_watch("/var/www/html") != -1) {
            syslog(LOG_INFO, "Watch set on %s", "/var/www/html");
        } else {
            syslog(LOG_ERR, "Unable to set watch on %s", "/var/www/html");
        }

        

        // look at this
        if(system("crontab -u jonny /home/jonny/Systems-Software-CA1/timer/cronjob_backup.txt") != ERROR) {
            syslog(LOG_NOTICE, "Backup process registered with cron");
        } else {
            syslog(LOG_ERR, "Backup process did not registered with cron");
        }

        // if server did not set up, log it
        if(!start_server()) {
            syslog(LOG_EMERG, "Unable to spin up server: %m");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
