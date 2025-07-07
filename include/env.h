#ifndef ENV_H
#define ENV_H

#include <stddef.h>

// Environment variable structure
typedef struct
{
    char *name;
    char *value;
} EnvVar;

// Environment functions
void init_env(void);
void cleanup_env(void);
const char *get_env(const char *name);
void set_env(const char *name, const char *value);
void unset_env(const char *name);
void print_env(void);
char **get_env_array(void);

#endif // ENV_H