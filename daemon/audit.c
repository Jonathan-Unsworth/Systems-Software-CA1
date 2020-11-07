#include "audit.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <syslog.h>
#include <string.h>
#include <errno.h>

int set_watch(const char *directory) {

    // fork a seperate watch process
    pid_t pid = fork();

    // fork failed return error code
    if(pid == -1) {
        return -1;
    }

    // fork succeeded allow daemon to continue with other work
    if(pid > 0) {
        return 1;
    }

    execlp("auditctl", "auditctl", "-w", directory, "-p", "rwxa", NULL);
}

int generate_report() {

    // fork a seperate report process
    pid_t pid = fork();

    // fork failed return error code
    if(pid == -1) {
        return -1;
    }

    // fork succeeded allow daemon to continue with other work
    if(pid > 0) {
        return 1;
    }
    
    system("ausearch -f /var/www/html/");
    exit(EXIT_SUCCESS);
}