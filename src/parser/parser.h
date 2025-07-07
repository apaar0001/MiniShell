#ifndef PARSER_H
#define PARSER_H

#include <sys/types.h>

#define MAX_ARGS 100
#define MAX_TOKENS 1024

typedef enum
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_BACKGROUND,
    TOKEN_EOF
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;

typedef struct
{
    char *command;
    char **args;
    int input_fd;
    int output_fd;
    int is_background;
} Command;

typedef struct CommandNode
{
    Command cmd;
    struct CommandNode *next;
} CommandNode;

// Token functions
Token *tokenize_input(const char *input);
void free_tokens(Token *tokens);

// Command parsing functions
CommandNode *parse_pipeline(Token *tokens);
void free_command_node(CommandNode *node);

// Quote handling
char *handle_quotes(const char *input);

#endif // PARSER_H