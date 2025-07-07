#include "../include/builtins.h"
#include "../include/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <readline/history.h>

// Built-in command implementations
int execute_cd(char **args)
{
    const char *dir;
    if (!args[1] || strcmp(args[1], "~") == 0)
    {
        dir = get_env("HOME");
        if (!dir)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }
    else
    {
        dir = args[1];
    }

    if (chdir(dir) != 0)
    {
        perror("cd");
        return 1;
    }
    return 0;
}

int execute_pwd(char **args)
{
    (void)args;
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
        return 0;
    }
    perror("pwd");
    return 1;
}

int execute_echo(char **args)
{
    int i = 1;
    int newline = 1;

    // Check for -n option
    if (args[1] && strcmp(args[1], "-n") == 0)
    {
        newline = 0;
        i = 2;
    }

    // Print arguments
    while (args[i])
    {
        if (args[i][0] == '$')
        {
            const char *value = get_env(args[i] + 1);
            if (value)
                printf("%s", value);
        }
        else
        {
            // Remove surrounding quotes if they exist
            char *arg = args[i];
            int len = strlen(arg);
            if (len >= 2 && ((arg[0] == '"' && arg[len - 1] == '"') ||
                             (arg[0] == '\'' && arg[len - 1] == '\'')))
            {
                arg[len - 1] = '\0';
                printf("%s", arg + 1);
            }
            else
            {
                printf("%s", arg);
            }
        }
        if (args[i + 1])
            printf(" ");
        i++;
    }

    if (newline)
        printf("\n");
    return 0;
}

int execute_exit(char **args)
{
    if (args[1])
    {
        int status = atoi(args[1]);
        exit(status);
    }
    exit(0);
    return 0;
}

int execute_export(char **args)
{
    if (!args[1])
    {
        print_env();
        return 0;
    }

    for (int i = 1; args[i]; i++)
    {
        char *equals = strchr(args[i], '=');
        if (!equals)
        {
            // Just export the variable without value
            set_env(args[i], "");
        }
        else
        {
            // Split at equals sign and set the variable
            *equals = '\0';
            set_env(args[i], equals + 1);
        }
    }
    return 0;
}

int execute_unset(char **args)
{
    if (!args[1])
    {
        fprintf(stderr, "unset: missing argument\n");
        return 1;
    }

    for (int i = 1; args[i]; i++)
    {
        unset_env(args[i]);
    }
    return 0;
}

int execute_env(char **args)
{
    (void)args;
    print_env();
    return 0;
}

int execute_history(char **args)
{
    (void)args;
    HIST_ENTRY **hist_list = history_list();
    if (!hist_list)
        return 0;

    for (int i = 0; hist_list[i]; i++)
    {
        printf("%5d  %s\n", i + 1, hist_list[i]->line);
    }
    return 0;
}