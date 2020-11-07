#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/file.h>
#include "daemonize.h"

#define ERROR -1
#define CHILD_PID 0

bool spawn_orphan();
bool fork_failed(pid_t pid);
bool is_parent(pid_t pid);
bool make_leader();
bool to_root();
void close_descriptors();

bool daemonize() {
    // make orphan process
    if(!spawn_orphan()) return false;

    // make the orphan the leader
    if(!make_leader()) return false;

    // orphan again
    if(!spawn_orphan()) return false;

    // set read/write permissions 
    umask(0);

    if(!to_root()) return false;

    // close file descriptors
    close_descriptors();

    return true;
}

bool spawn_orphan() {
    pid_t pid = fork();

    if(fork_failed(pid)) return false; 

    if(is_parent(pid)) exit(EXIT_SUCCESS);
    
    return true;
}

bool fork_failed(pid_t pid) {
    return pid == ERROR;
}

bool is_parent(pid_t pid) {
    return pid > CHILD_PID;
}

bool make_leader() {
    return setsid() != ERROR;
}

bool to_root() {
    return chdir("/") != ERROR;
}

void close_descriptors() {
    for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--) {
        close(fd);
    }
}