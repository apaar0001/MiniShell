#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>

// String manipulation functions
size_t my_strlen(const char *str);
char *my_strdup(const char *str);
char *my_strndup(const char *str, size_t n);
int my_strcmp(const char *s1, const char *s2);
char *my_strtok(char *str, const char *delim);

#endif // STRING_UTILS_H