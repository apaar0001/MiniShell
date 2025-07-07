#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "readline.h"
#include "parser.h"
#include "env.h"
#include "builtins.h"
#include "execute.h"

// Shell configuration
#define MAX_ARGS 100
#define MAX_PIPES 10
#define MAX_REDIRECTS 10

// Shell state
extern int exit_status;
extern int running;

// Shell functions
void init_shell(void);
void cleanup_shell(void);
void execute_script(const char *filename);
const char *get_prompt(void);

// Signal handlers
void handle_signal(int sig);
void handle_sigquit(int sig);
void handle_sigtstp(int sig);
void handle_sigchld(int sig);

// Environment functions
void init_env(void);
void cleanup_env(void);

#endif // SHELL_H
