#ifndef ALIAS_H
#define ALIAS_H

// Alias management functions
void init_aliases(void);
void cleanup_aliases(void);
int add_alias(const char *name, const char *value);
int remove_alias(const char *name);
const char *get_alias(const char *name);
void print_aliases(void);
char *expand_aliases(const char *input);

#endif // ALIAS_H