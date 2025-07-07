#include "../include/shell.h"
#include "../include/parser.h"
#include "../include/readline.h"
#include "../include/env.h"
#include "../include/builtins.h"
#include "../include/execute.h"
#include "../include/history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

// Global variables
static LineEditor editor;
int exit_status = 0;
int running = 1;

#define PROMPT "minishell$ "

// Get prompt string
const char *get_prompt(void)
{
    return PROMPT;
}

// Signal handlers
void handle_signal(int sig)
{
    if (sig == SIGINT)
    {
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

void handle_sigquit(int sig)
{
    (void)sig;
    printf("\n");
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void handle_sigtstp(int sig)
{
    (void)sig;
    printf("\n");
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void handle_sigchld(int sig)
{
    (void)sig;
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // Clean up completed child processes
    }
}

void init_shell(void)
{
    // Set up signal handlers
    signal(SIGINT, handle_signal);
    signal(SIGQUIT, handle_sigquit);
    signal(SIGTSTP, handle_sigtstp);
    signal(SIGCHLD, handle_sigchld);

    // Initialize readline
    rl_readline_name = "minishell";
    rl_attempted_completion_function = NULL;
    using_history();

    // Initialize components
    init_line_editor(&editor);
    init_env();
}

void cleanup_shell(void)
{
    cleanup_line_editor(&editor);
    cleanup_env();
}

void execute_script(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening script file");
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file))
    {
        // Remove newline
        line[strcspn(line, "\n")] = 0;

        // Skip empty lines and comments
        if (line[0] == 0 || line[0] == '#')
            continue;

        // Parse and execute the command
        CommandNode *ast = parse_command(line);
        if (ast)
        {
            execute_pipeline(ast);
            free_command(ast);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    init_shell();

    if (argc > 1)
    {
        execute_script(argv[1]);
        cleanup_shell();
        return 0;
    }

    char *line;
    CommandNode *ast;
    int status = 0;

    while (running)
    {
        // Read command line
        line = get_input_line(PROMPT);
        if (!line)
        {
            printf("exit\n");
            break;
        }

        // Skip empty lines
        if (line[0] == '\0')
        {
            free(line);
            continue;
        }

        // Parse command
        ast = parse_command(line);
        if (ast)
        {
            // Execute command
            status = execute_pipeline(ast);
            free_command(ast);

            // Check for exit command
            if (strcmp(line, "exit") == 0)
            {
                free(line);
                break;
            }
        }

        free(line);
    }

    cleanup_shell();
    return status;
}