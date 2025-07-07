#include "../include/alias.h"
#include "../utils/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALIASES 100

static Alias aliases[MAX_ALIASES];
static int alias_count = 0;

void init_aliases(void)
{
    // Add some default aliases
    add_alias("ll", "ls -l");
    add_alias("la", "ls -la");
    add_alias("grep", "grep --color=auto");
}

void cleanup_aliases(void)
{
    for (int i = 0; i < alias_count; i++)
    {
        free(aliases[i].name);
        free(aliases[i].value);
    }
    alias_count = 0;
}

const char *get_alias(const char *name)
{
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(aliases[i].name, name) == 0)
        {
            return aliases[i].value;
        }
    }
    return NULL;
}

void add_alias(const char *name, const char *value)
{
    // Check if alias already exists
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(aliases[i].name, name) == 0)
        {
            free(aliases[i].value);
            aliases[i].value = my_strdup(value);
            return;
        }
    }

    // Add new alias
    if (alias_count < MAX_ALIASES)
    {
        aliases[alias_count].name = my_strdup(name);
        aliases[alias_count].value = my_strdup(value);
        alias_count++;
    }
}

void remove_alias(const char *name)
{
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(aliases[i].name, name) == 0)
        {
            free(aliases[i].name);
            free(aliases[i].value);
            for (int j = i; j < alias_count - 1; j++)
            {
                aliases[j] = aliases[j + 1];
            }
            alias_count--;
            return;
        }
    }
}

void print_aliases(void)
{
    for (int i = 0; i < alias_count; i++)
    {
        printf("alias %s='%s'\n", aliases[i].name, aliases[i].value);
    }
}

char *expand_aliases(const char *input)
{
    char *result = my_strdup(input);
    if (!result)
        return NULL;

    char *token = my_strtok(result, " \t\n");
    if (!token)
    {
        free(result);
        return NULL;
    }

    const char *alias_value = get_alias(token);
    if (alias_value)
    {
        size_t new_len = strlen(alias_value) + strlen(input) - strlen(token);
        char *new_result = malloc(new_len + 1);
        if (!new_result)
        {
            free(result);
            return NULL;
        }

        strcpy(new_result, alias_value);
        strcat(new_result, input + strlen(token));
        free(result);
        result = new_result;
    }

    return result;
}