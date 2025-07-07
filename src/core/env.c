#include "../include/env.h"
#include "../utils/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENV_VARS 1000

static EnvVar env_vars[MAX_ENV_VARS];
static int env_count = 0;

void init_env(void)
{
    // Copy environment variables from system
    extern char **environ;
    char **env = environ;

    // If environ is NULL or empty, set up some basic environment variables
    if (!env || !*env)
    {
        set_env("PATH", "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");
        set_env("HOME", getenv("HOME") ? getenv("HOME") : "/");
        set_env("USER", getenv("USER") ? getenv("USER") : "user");
        set_env("SHELL", "/bin/minishell");
        return;
    }

    while (*env && env_count < MAX_ENV_VARS)
    {
        char *name = strdup(*env);
        if (!name)
            continue;

        char *value = strchr(name, '=');
        if (value)
        {
            *value = '\0';
            value++;
            env_vars[env_count].name = name;
            env_vars[env_count].value = strdup(value);
            if (env_vars[env_count].value)
                env_count++;
            else
                free(name);
        }
        else
        {
            free(name);
        }
        env++;
    }

    // Ensure PATH is set
    if (!get_env("PATH"))
    {
        set_env("PATH", "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");
    }
}

void cleanup_env(void)
{
    for (int i = 0; i < env_count; i++)
    {
        free(env_vars[i].name);
        free(env_vars[i].value);
    }
    env_count = 0;
}

const char *get_env(const char *name)
{
    for (int i = 0; i < env_count; i++)
    {
        if (strcmp(env_vars[i].name, name) == 0)
        {
            return env_vars[i].value;
        }
    }
    return NULL;
}

void set_env(const char *name, const char *value)
{
    // Check if variable already exists
    for (int i = 0; i < env_count; i++)
    {
        if (strcmp(env_vars[i].name, name) == 0)
        {
            free(env_vars[i].value);
            env_vars[i].value = strdup(value);
            return;
        }
    }

    // Add new variable
    if (env_count < MAX_ENV_VARS)
    {
        env_vars[env_count].name = strdup(name);
        env_vars[env_count].value = strdup(value);
        env_count++;
    }
}

void unset_env(const char *name)
{
    for (int i = 0; i < env_count; i++)
    {
        if (strcmp(env_vars[i].name, name) == 0)
        {
            free(env_vars[i].name);
            free(env_vars[i].value);

            // Shift remaining variables
            for (int j = i; j < env_count - 1; j++)
            {
                env_vars[j] = env_vars[j + 1];
            }
            env_count--;
            return;
        }
    }
}

void print_env(void)
{
    for (int i = 0; i < env_count; i++)
    {
        printf("%s=%s\n", env_vars[i].name, env_vars[i].value);
    }
}

char **get_env_array(void)
{
    char **array = malloc(sizeof(char *) * (env_count + 1));
    if (!array)
        return NULL;

    for (int i = 0; i < env_count; i++)
    {
        size_t len = strlen(env_vars[i].name) + strlen(env_vars[i].value) + 2;
        array[i] = malloc(len);
        if (!array[i])
        {
            for (int j = 0; j < i; j++)
            {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
        snprintf(array[i], len, "%s=%s", env_vars[i].name, env_vars[i].value);
    }
    array[env_count] = NULL;
    return array;
}