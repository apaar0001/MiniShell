#include "../include/readline.h"
#include "../include/history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

void init_line_editor(LineEditor *editor)
{
    if (!editor)
        return;

    // Initialize line editor state
    editor->line[0] = '\0';
    editor->line_length = 0;
    editor->cursor_pos = 0;
    editor->history_count = 0;
    editor->history_index = 0;

    // Initialize readline
    rl_readline_name = "minishell";
    rl_attempted_completion_function = NULL;
    using_history();
}

void cleanup_line_editor(LineEditor *editor)
{
    if (!editor)
        return;

    // No cleanup needed for readline
    (void)editor;
}

void clear_line(LineEditor *editor)
{
    if (!editor)
        return;

    // No need to clear line as readline handles this
    (void)editor;
}

char *get_input_line(const char *prompt)
{
    if (!prompt)
        return NULL;

    char *line;

    // Use readline library for input with history support
    line = readline(prompt);
    if (!line)
        return NULL;

    // Add non-empty lines to history
    if (line[0] != '\0')
    {
        add_history(line);
    }

    return line;
}