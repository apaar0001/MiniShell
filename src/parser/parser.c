#include "../../include/parser.h"
#include "../../include/env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ARGS 100

// Token types
typedef enum
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND
} TokenType;

// Token structure
typedef struct
{
    char *value;
    TokenType type;
} Token;

// Helper function to skip whitespace
static const char *skip_whitespace(const char *p)
{
    while (*p && isspace(*p))
        p++;
    return p;
}

// Helper function to get word length
static int get_word_length(const char *p)
{
    int len = 0;
    while (*p && !isspace(*p) && *p != '|' && *p != '<' && *p != '>')
    {
        if (*p == '"' || *p == '\'')
        {
            char quote = *p++;
            len++;
            while (*p && *p != quote)
            {
                p++;
                len++;
            }
            if (*p == quote)
            {
                p++;
                len++;
            }
        }
        else
        {
            p++;
            len++;
        }
    }
    return len;
}

// Helper function to expand environment variables
static char *expand_env_var(const char *word)
{
    if (!word || word[0] != '$')
        return strdup(word);

    const char *var_name = word + 1;
    if (!*var_name) // Handle case where $ is the last character
        return strdup("$");

    // Find the end of the variable name
    const char *end = var_name;
    while (*end && (isalnum(*end) || *end == '_'))
        end++;

    // If no valid variable name was found, return the original string
    if (end == var_name)
        return strdup(word);

    // Create a temporary string for the variable name
    int var_len = end - var_name;
    char *name = malloc(var_len + 1);
    if (!name)
        return strdup(word);

    strncpy(name, var_name, var_len);
    name[var_len] = '\0';

    // Get the environment variable value
    const char *value = get_env(name);
    free(name);

    if (!value)
        return strdup(""); // Return empty string if variable not found

    return strdup(value);
}

// Helper function to copy word with environment variable expansion
static char *copy_word(const char *p, int len)
{
    char *word = malloc(len + 1);
    if (!word)
        return NULL;

    int i = 0;
    while (i < len)
    {
        if (*p == '"' || *p == '\'')
        {
            char quote = *p++;
            word[i++] = quote;
            while (*p && *p != quote)
            {
                word[i++] = *p++;
            }
            if (*p == quote)
            {
                word[i++] = *p++;
            }
        }
        else if (*p == '$')
        {
            // Find the end of the variable name
            const char *var_start = p;
            p++;
            while (*p && (isalnum(*p) || *p == '_'))
                p++;

            // Create temporary string for variable name
            int var_len = p - var_start;
            char *var_name = malloc(var_len + 1);
            if (!var_name)
            {
                free(word);
                return NULL;
            }
            strncpy(var_name, var_start, var_len);
            var_name[var_len] = '\0';

            // Expand the variable
            char *expanded = expand_env_var(var_name);
            if (expanded)
            {
                strcpy(word + i, expanded);
                i += strlen(expanded);
                free(expanded);
            }
            free(var_name);
        }
        else
        {
            word[i++] = *p++;
        }
    }
    word[i] = '\0';
    return word;
}

// Tokenize input string
static Token *tokenize(const char *input, int *token_count)
{
    Token *tokens = malloc(MAX_ARGS * sizeof(Token));
    if (!tokens)
        return NULL;

    *token_count = 0;
    const char *p = input;

    while (*p)
    {
        p = skip_whitespace(p);
        if (!*p)
            break;

        if (*p == '|')
        {
            tokens[*token_count].type = TOKEN_PIPE;
            tokens[*token_count].value = strdup("|");
            (*token_count)++;
            p++;
        }
        else if (*p == '<')
        {
            tokens[*token_count].type = TOKEN_REDIRECT_IN;
            tokens[*token_count].value = strdup("<");
            (*token_count)++;
            p++;
        }
        else if (*p == '>')
        {
            p++;
            if (*p == '>')
            {
                tokens[*token_count].type = TOKEN_REDIRECT_APPEND;
                tokens[*token_count].value = strdup(">>");
                (*token_count)++;
                p++;
            }
            else
            {
                tokens[*token_count].type = TOKEN_REDIRECT_OUT;
                tokens[*token_count].value = strdup(">");
                (*token_count)++;
            }
        }
        else
        {
            int len = get_word_length(p);
            tokens[*token_count].type = TOKEN_WORD;
            tokens[*token_count].value = copy_word(p, len);
            (*token_count)++;
            p += len;
        }
    }

    return tokens;
}

// Free tokens
static void free_tokens(Token *tokens, int count)
{
    for (int i = 0; i < count; i++)
    {
        free(tokens[i].value);
    }
    free(tokens);
}

// Parse command line into command structure
CommandNode *parse_command(const char *line)
{
    int token_count;
    Token *tokens = tokenize(line, &token_count);
    if (!tokens)
        return NULL;

    CommandNode *head = NULL;
    CommandNode *current = NULL;
    CommandNode *prev = NULL;

    for (int i = 0; i < token_count; i++)
    {
        if (tokens[i].type == TOKEN_PIPE)
        {
            if (current)
            {
                current->next = malloc(sizeof(CommandNode));
                if (!current->next)
                {
                    free_tokens(tokens, token_count);
                    free_command(head);
                    return NULL;
                }
                prev = current;
                current = current->next;
                current->args = malloc(MAX_ARGS * sizeof(char *));
                if (!current->args)
                {
                    free_tokens(tokens, token_count);
                    free_command(head);
                    return NULL;
                }
                current->args[0] = NULL;
                current->input_file = NULL;
                current->output_file = NULL;
                current->append_output = 0;
                current->next = NULL;
            }
        }
        else if (tokens[i].type == TOKEN_WORD)
        {
            if (!current)
            {
                current = malloc(sizeof(CommandNode));
                if (!current)
                {
                    free_tokens(tokens, token_count);
                    return NULL;
                }
                current->args = malloc(MAX_ARGS * sizeof(char *));
                if (!current->args)
                {
                    free_tokens(tokens, token_count);
                    free(current);
                    return NULL;
                }
                current->args[0] = NULL;
                current->input_file = NULL;
                current->output_file = NULL;
                current->append_output = 0;
                current->next = NULL;
                head = current;
            }

            int arg_count = 0;
            while (current->args[arg_count])
                arg_count++;
            current->args[arg_count] = strdup(tokens[i].value);
            current->args[arg_count + 1] = NULL;
        }
        else if (tokens[i].type == TOKEN_REDIRECT_IN)
        {
            if (i + 1 < token_count && tokens[i + 1].type == TOKEN_WORD)
            {
                current->input_file = strdup(tokens[i + 1].value);
                i++;
            }
        }
        else if (tokens[i].type == TOKEN_REDIRECT_OUT || tokens[i].type == TOKEN_REDIRECT_APPEND)
        {
            if (i + 1 < token_count && tokens[i + 1].type == TOKEN_WORD)
            {
                current->output_file = strdup(tokens[i + 1].value);
                current->append_output = (tokens[i].type == TOKEN_REDIRECT_APPEND);
                i++;
            }
        }
    }

    free_tokens(tokens, token_count);
    return head;
}

// Free command structure
void free_command(CommandNode *cmd)
{
    while (cmd)
    {
        CommandNode *next = cmd->next;
        if (cmd->args)
        {
            for (int i = 0; cmd->args[i]; i++)
            {
                free(cmd->args[i]);
            }
            free(cmd->args);
        }
        free(cmd->input_file);
        free(cmd->output_file);
        free(cmd);
        cmd = next;
    }
}