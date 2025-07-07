#ifndef ENV_H
#define ENV_H

#include <stddef.h>

// Environment variable structure
typedef struct
{
    char *name;
    char *value;
} EnvVar;

// Environment management functions
void init_environment(void);
void cleanup_environment(void);
const char *get_env(const char *name);
int set_env(const char *name, const char *value);
int unset_env(const char *name);
void print_environment(void);
char **get_env_array(void);

#endif // ENV_H