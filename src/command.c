#include "../include/shell.h"
#include "../include/parser.h"

void parse_command(char *line, Command *cmd)
{
    char *token;
    int i = 0;

    // Initialize command structure
    cmd->command = NULL;
    cmd->args = malloc(sizeof(char *) * MAX_ARGS);
    cmd->input_fd = STDIN_FILENO;
    cmd->output_fd = STDOUT_FILENO;
    cmd->is_background = 0;

    // Tokenize the command line
    token = strtok(line, " \t\n");
    if (token)
    {
        cmd->command = strdup(token);
        cmd->args[i++] = strdup(token);

        while ((token = strtok(NULL, " \t\n")) && i < MAX_ARGS - 1)
        {
            if (strcmp(token, "&") == 0)
            {
                cmd->is_background = 1;
                break;
            }
            cmd->args[i++] = strdup(token);
        }
    }
    cmd->args[i] = NULL;
}

void execute_pipeline(CommandNode *pipeline)
{
    if (!pipeline)
        return;

    // Single command case
    if (!pipeline->next)
    {
        execute_command(&pipeline->cmd);
        return;
    }

    // Multiple commands in pipeline
    int pipes[2];
    pid_t pid;
    CommandNode *current = pipeline;

    while (current)
    {
        if (current->next)
        {
            if (pipe(pipes) < 0)
            {
                perror("pipe");
                return;
            }
        }

        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return;
        }

        if (pid == 0)
        {
            // Child process
            if (current->next)
            {
                // Not the last command
                close(pipes[0]);
                dup2(pipes[1], STDOUT_FILENO);
                close(pipes[1]);
            }

            if (current != pipeline)
            {
                // Not the first command
                dup2(current->cmd.input_fd, STDIN_FILENO);
            }

            // Handle I/O redirection
            if (current->cmd.input_fd != STDIN_FILENO)
            {
                dup2(current->cmd.input_fd, STDIN_FILENO);
                close(current->cmd.input_fd);
            }
            if (current->cmd.output_fd != STDOUT_FILENO)
            {
                dup2(current->cmd.output_fd, STDOUT_FILENO);
                close(current->cmd.output_fd);
            }

            execvp(current->cmd.command, current->cmd.args);
            perror("execvp");
            exit(1);
        }

        // Parent process
        if (current->next)
        {
            close(pipes[1]);
            current->next->cmd.input_fd = pipes[0];
        }

        if (!current->cmd.is_background)
        {
            int status;
            waitpid(pid, &status, 0);
        }
        else
        {
            add_job(pid, current->cmd.command);
        }

        current = current->next;
    }
}

void execute_command(Command *cmd)
{
    if (!cmd->command)
        return;

    // Check for built-in commands
    if (strcmp(cmd->command, "cd") == 0)
    {
        builtin_cd(cmd);
        return;
    }
    if (strcmp(cmd->command, "exit") == 0)
    {
        builtin_exit(cmd);
        return;
    }
    if (strcmp(cmd->command, "pwd") == 0)
    {
        builtin_pwd(cmd);
        return;
    }
    if (strcmp(cmd->command, "echo") == 0)
    {
        builtin_echo(cmd);
        return;
    }
    if (strcmp(cmd->command, "history") == 0)
    {
        builtin_history(cmd);
        return;
    }
    if (strcmp(cmd->command, "jobs") == 0)
    {
        list_jobs();
        return;
    }
    if (strcmp(cmd->command, "fg") == 0)
    {
        if (cmd->args[1])
        {
            int job_id = atoi(cmd->args[1]);
            bring_to_foreground(job_id);
        }
        return;
    }
    if (strcmp(cmd->command, "bg") == 0)
    {
        if (cmd->args[1])
        {
            int job_id = atoi(cmd->args[1]);
            continue_background(job_id);
        }
        return;
    }

    // Execute external command
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        // Child process
        if (cmd->input_fd != STDIN_FILENO)
        {
            dup2(cmd->input_fd, STDIN_FILENO);
            close(cmd->input_fd);
        }
        if (cmd->output_fd != STDOUT_FILENO)
        {
            dup2(cmd->output_fd, STDOUT_FILENO);
            close(cmd->output_fd);
        }

        execvp(cmd->command, cmd->args);
        perror("execvp");
        exit(1);
    }
    else
    {
        // Parent process
        if (!cmd->is_background)
        {
            int status;
            waitpid(pid, &status, 0);
        }
        else
        {
            add_job(pid, cmd->command);
        }
    }
}

void setup_io_redirection(Command *cmd)
{
    for (int i = 0; cmd->args[i]; i++)
    {
        if (strcmp(cmd->args[i], ">") == 0)
        {
            if (cmd->args[i + 1])
            {
                cmd->output_fd = open(cmd->args[i + 1],
                                      O_WRONLY | O_CREAT | O_TRUNC,
                                      0644);
                cmd->args[i] = NULL;
            }
        }
        else if (strcmp(cmd->args[i], ">>") == 0)
        {
            if (cmd->args[i + 1])
            {
                cmd->output_fd = open(cmd->args[i + 1],
                                      O_WRONLY | O_CREAT | O_APPEND,
                                      0644);
                cmd->args[i] = NULL;
            }
        }
        else if (strcmp(cmd->args[i], "<") == 0)
        {
            if (cmd->args[i + 1])
            {
                cmd->input_fd = open(cmd->args[i + 1], O_RDONLY);
                cmd->args[i] = NULL;
            }
        }
    }
}