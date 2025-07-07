#include "../include/history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#define HISTORY_FILE ".minishell_history"

void init_history(LineEditor *editor)
{
    if (!editor)
        return;

    // Initialize line editor state
    editor->history_count = 0;
    editor->history_index = 0;

    // Load history from file
    load_history(editor);
}

void add_to_history(LineEditor *editor, const char *line)
{
    if (!editor || !line || line[0] == '\0')
        return;

    // Add to readline history
    add_history(line);
    editor->history_count++;
}

void save_history(LineEditor *editor)
{
    if (!editor)
        return;

    char *home = getenv("HOME");
    if (!home)
        return;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s", home, HISTORY_FILE);

    // Save readline history
    write_history(path);
}

void load_history(LineEditor *editor)
{
    if (!editor)
        return;

    char *home = getenv("HOME");
    if (!home)
        return;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s", home, HISTORY_FILE);

    // Load readline history
    read_history(path);
    editor->history_count = history_length;
}

void print_history(LineEditor *editor)
{
    if (!editor)
        return;

    HIST_ENTRY **entries = history_list();
    if (!entries)
        return;

    for (int i = 0; entries[i]; i++)
    {
        printf("%d %s\n", i + 1, entries[i]->line);
    }
}

char *search_history(LineEditor *editor, const char *pattern)
{
    if (!editor || !pattern)
        return NULL;

    HIST_ENTRY **entries = history_list();
    if (!entries)
        return NULL;

    for (int i = 0; entries[i]; i++)
    {
        if (strstr(entries[i]->line, pattern))
        {
            return strdup(entries[i]->line);
        }
    }

    return NULL;
}

void cleanup_history(LineEditor *editor)
{
    if (!editor)
        return;

    // Save history before cleanup
    save_history(editor);
    editor->history_count = 0;
    editor->history_index = 0;
}