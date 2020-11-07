#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#define ERROR -1
#define CHILD_PID 0

int main(int argc, char *argv[]) {
    syslog(LOG_NOTICE, "Backup process executed");

    if(argc != 3) {
        syslog(LOG_ERR, "usage: %s [from] [to]", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if(pid == ERROR) {
        syslog(LOG_ERR, "fork failed on backup");
        exit(EXIT_FAILURE);
    }

    // need to lock directory
    if(pid == CHILD_PID)
        execlp("cp", "cp", "-a", argv[1], argv[2], NULL);

    // wait for child process to fin
    // unlock directory
    syslog(LOG_INFO, "backup process finished");
    
    return 0;
}