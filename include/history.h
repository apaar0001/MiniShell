#ifndef HISTORY_H
#define HISTORY_H

#include "readline.h"
#include <readline/readline.h>
#include <readline/history.h>

// Initialize history
void init_history(LineEditor *editor);

// Add a line to history
void add_to_history(LineEditor *editor, const char *line);

// Save history to file
void save_history(LineEditor *editor);

// Load history from file
void load_history(LineEditor *editor);

// Print history
void print_history(LineEditor *editor);

// Search history
char *search_history(LineEditor *editor, const char *pattern);

// Clean up history
void cleanup_history(LineEditor *editor);

#endif // HISTORY_H