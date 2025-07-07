#ifndef ALIAS_H
#define ALIAS_H

#include <stddef.h>

// Alias structure
typedef struct
{
    char *name;
    char *value;
} Alias;

// Alias management functions
void init_aliases(void);
void cleanup_aliases(void);
const char *get_alias(const char *name);
void add_alias(const char *name, const char *value);
void remove_alias(const char *name);
void print_aliases(void);
char *expand_aliases(const char *input);

#endif // ALIAS_H