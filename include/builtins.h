#ifndef BUILTINS_H
#define BUILTINS_H

// Built-in command function declarations
int execute_cd(char **args);
int execute_pwd(char **args);
int execute_echo(char **args);
int execute_exit(char **args);
int execute_export(char **args);
int execute_unset(char **args);
int execute_env(char **args);
int execute_history(char **args);

#endif // BUILTINS_H