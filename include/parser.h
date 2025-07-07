#ifndef PARSER_H
#define PARSER_H

// Command node structure
typedef struct CommandNode
{
    char **args;              // Command arguments
    char *input_file;         // Input redirection file
    char *output_file;        // Output redirection file
    int append_output;        // Whether to append to output file
    struct CommandNode *next; // Next command in pipeline
} CommandNode;

// Parse a command line into a command structure
CommandNode *parse_command(const char *line);

// Free a command structure
void free_command(CommandNode *cmd);

#endif // PARSER_H