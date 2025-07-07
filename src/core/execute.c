#include "../include/parser.h"
#include "../include/builtins.h"
#include "../include/env.h"
#include "../include/readline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

// Built-in command table
typedef struct
{
    const char *name;
    int (*func)(char **);
} BuiltinCmd;

static BuiltinCmd builtins[] = {
    {"cd", execute_cd},
    {"pwd", execute_pwd},
    {"echo", execute_echo},
    {"exit", execute_exit},
    {"export", execute_export},
    {"unset", execute_unset},
    {"env", execute_env},
    {"history", execute_history},
    {NULL, NULL}};

static int is_builtin(const char *cmd)
{
    const char *builtins[] = {"cd", "pwd", "echo", "exit", "export", "unset", "env", "history", NULL};
    for (int i = 0; builtins[i]; i++)
    {
        if (strcmp(cmd, builtins[i]) == 0)
            return 1;
    }
    return 0;
}

static int execute_builtin(char **args)
{
    if (strcmp(args[0], "cd") == 0)
        return execute_cd(args);
    else if (strcmp(args[0], "pwd") == 0)
        return execute_pwd(args);
    else if (strcmp(args[0], "echo") == 0)
        return execute_echo(args);
    else if (strcmp(args[0], "exit") == 0)
        return execute_exit(args);
    else if (strcmp(args[0], "export") == 0)
        return execute_export(args);
    else if (strcmp(args[0], "unset") == 0)
        return execute_unset(args);
    else if (strcmp(args[0], "env") == 0)
        return execute_env(args);
    else if (strcmp(args[0], "history") == 0)
        return execute_history(args);
    return 1;
}

static int handle_redirection(char **args, int *in_fd, int *out_fd)
{
    *in_fd = STDIN_FILENO;
    *out_fd = STDOUT_FILENO;

    for (int i = 0; args[i]; i++)
    {
        if (strcmp(args[i], "<") == 0)
        {
            if (!args[i + 1])
            {
                fprintf(stderr, "minishell: syntax error near unexpected token 'newline'\n");
                return 1;
            }
            *in_fd = open(args[i + 1], O_RDONLY);
            if (*in_fd == -1)
            {
                fprintf(stderr, "minishell: %s: %s\n", args[i + 1], strerror(errno));
                return 1;
            }
            args[i] = NULL;
            args[i + 1] = NULL;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            if (!args[i + 1])
            {
                fprintf(stderr, "minishell: syntax error near unexpected token 'newline'\n");
                return 1;
            }
            *out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (*out_fd == -1)
            {
                fprintf(stderr, "minishell: %s: %s\n", args[i + 1], strerror(errno));
                return 1;
            }
            args[i] = NULL;
            args[i + 1] = NULL;
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            if (!args[i + 1])
            {
                fprintf(stderr, "minishell: syntax error near unexpected token 'newline'\n");
                return 1;
            }
            *out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (*out_fd == -1)
            {
                fprintf(stderr, "minishell: %s: %s\n", args[i + 1], strerror(errno));
                return 1;
            }
            args[i] = NULL;
            args[i + 1] = NULL;
        }
    }
    return 0;
}

static int handle_pipe(char **args)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    }
    else
    {
        // Parent process
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        waitpid(pid, NULL, 0);
    }
    return 0;
}

int execute_command(char **args)
{
    if (!args || !args[0])
        return 0;

    // Handle pipes
    for (int i = 0; args[i]; i++)
    {
        if (strcmp(args[i], "|") == 0)
        {
            args[i] = NULL;
            return handle_pipe(args);
        }
    }

    // Handle redirections
    int in_fd, out_fd;
    if (handle_redirection(args, &in_fd, &out_fd) != 0)
        return 1;

    // Execute built-in commands
    if (is_builtin(args[0]))
    {
        // Save original file descriptors
        int original_stdin = dup(STDIN_FILENO);
        int original_stdout = dup(STDOUT_FILENO);
        if (original_stdin == -1 || original_stdout == -1)
        {
            perror("dup");
            return 1;
        }

        // Set up redirection
        if (in_fd != STDIN_FILENO)
        {
            if (dup2(in_fd, STDIN_FILENO) == -1)
            {
                perror("dup2");
                close(original_stdin);
                close(original_stdout);
                return 1;
            }
            close(in_fd);
        }
        if (out_fd != STDOUT_FILENO)
        {
            if (dup2(out_fd, STDOUT_FILENO) == -1)
            {
                perror("dup2");
                close(original_stdin);
                close(original_stdout);
                return 1;
            }
            close(out_fd);
        }

        // Execute the built-in command
        int status = execute_builtin(args);

        // Restore original file descriptors
        if (dup2(original_stdin, STDIN_FILENO) == -1)
        {
            perror("dup2");
            close(original_stdin);
            close(original_stdout);
            return 1;
        }
        if (dup2(original_stdout, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(original_stdin);
            close(original_stdout);
            return 1;
        }
        close(original_stdin);
        close(original_stdout);

        return status;
    }

    // Execute external commands
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        // Child process
        if (in_fd != STDIN_FILENO)
        {
            if (dup2(in_fd, STDIN_FILENO) == -1)
            {
                perror("dup2");
                exit(1);
            }
            close(in_fd);
        }
        if (out_fd != STDOUT_FILENO)
        {
            if (dup2(out_fd, STDOUT_FILENO) == -1)
            {
                perror("dup2");
                exit(1);
            }
            close(out_fd);
        }

        // Check if command exists in PATH
        const char *path = get_env("PATH");
        if (!path)
        {
            fprintf(stderr, "minishell: PATH environment variable not set\n");
            exit(1);
        }

        // Try to execute the command
        execvp(args[0], args);

        // If execvp fails, check if it's because the command wasn't found
        if (errno == ENOENT)
        {
            fprintf(stderr, "minishell: %s: command not found\n", args[0]);
        }
        else
        {
            fprintf(stderr, "minishell: %s: %s\n", args[0], strerror(errno));
        }
        exit(1);
    }
    else
    {
        // Parent process
        if (in_fd != STDIN_FILENO)
            close(in_fd);
        if (out_fd != STDOUT_FILENO)
            close(out_fd);

        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
}

// Execute a pipeline of commands
int execute_pipeline(CommandNode *pipeline)
{
    if (!pipeline)
        return 0;

    // If it's a single command, execute it directly
    if (!pipeline->next)
    {
        return execute_command(pipeline->args);
    }

    // Create pipe
    int pipefd[2];
    if (pipe(pipefd) < 0)
    {
        perror("pipe");
        return 1;
    }

    // Fork for first command
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    else if (pid == 0)
    {
        // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execute_command(pipeline->args);
        exit(0);
    }

    // Fork for second command
    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    else if (pid2 == 0)
    {
        // Child process
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execute_command(pipeline->next->args);
        exit(0);
    }

    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for both children
    int status1, status2;
    waitpid(pid, &status1, 0);
    waitpid(pid2, &status2, 0);

    return WEXITSTATUS(status2);
}

// Execute a script file
int execute_script_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening script file");
        return 1;
    }

    char line[MAX_LINE];
    int status = 0;

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
            status = execute_pipeline(ast);
            free_command(ast);
        }
    }

    fclose(file);
    return status;
}