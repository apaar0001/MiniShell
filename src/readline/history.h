#ifndef HISTORY_H
#define HISTORY_H

#include <time.h>

#define MAX_HISTORY 1000
#define HISTORY_FILE ".minishell_history"

typedef struct
{
    char *line;
    time_t timestamp;
    int exit_status;
} HistoryEntry;

// History management functions
void init_history(void);
void cleanup_history(void);
void add_to_history(const char *line, int exit_status);
void save_history(void);
void load_history(void);
void print_history(void);
char *search_history(const char *pattern);
char *get_history_entry(int index);
int get_history_count(void);

#endif // HISTORY_H