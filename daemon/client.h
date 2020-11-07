#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

int connect_to_daemon();
bool send_command(int send_fd, const char *command);
bool command_acknowledged(int send_fd);
char* parse_commandline(int argc, char **argv); 

#endif