#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"

// Execute a single command
int execute_command(char **args);

// Execute a pipeline of commands
int execute_pipeline(CommandNode *pipeline);

// Execute a script file
int execute_script_file(const char *filename);

#endif // EXECUTE_H