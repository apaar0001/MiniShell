#ifndef COMMAND_H
#define COMMAND_H

#include <unistd.h>

#define MAX_ARGS 100

typedef struct
{
    char *command;
    char *args[MAX_ARGS];
    int input_fd;
    int output_fd;
    int is_background;
} Command;

#endif // COMMAND_H