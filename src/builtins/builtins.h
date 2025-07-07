#ifndef BUILTINS_H
#define BUILTINS_H

#include "../parser/parser.h"

// Built-in command functions
int execute_cd(Command *cmd);
int execute_pwd(Command *cmd);
int execute_exit(Command *cmd);
int execute_echo(Command *cmd);
int execute_jobs(Command *cmd);
int execute_fg(Command *cmd);
int execute_bg(Command *cmd);

// New built-in commands
int execute_export(Command *cmd);
int execute_unset(Command *cmd);
int execute_env(Command *cmd);
int execute_alias(Command *cmd);
int execute_unalias(Command *cmd);
int execute_history(Command *cmd);
int execute_help(Command *cmd);

// Built-in command handler
int execute_builtin(Command *cmd);

#endif // BUILTINS_H