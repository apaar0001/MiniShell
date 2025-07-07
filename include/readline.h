#ifndef READLINE_H
#define READLINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_LINE 4096

typedef struct
{
    char line[MAX_LINE];
    int line_length;
    int cursor_pos;
    int history_count;
    int history_index;
} LineEditor;

// Initialize line editor
void init_line_editor(LineEditor *editor);

// Clean up line editor
void cleanup_line_editor(LineEditor *editor);

// Read a line of input
char *get_input_line(const char *prompt);

// Clear the current line
void clear_line(LineEditor *editor);

#endif // READLINE_H
