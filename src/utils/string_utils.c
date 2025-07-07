#include "../include/string_utils.h"
#include <stdlib.h>
#include <string.h>

size_t my_strlen(const char *s)
{
    const char *p = s;
    while (*p)
        p++;
    return p - s;
}

char *my_strdup(const char *s)
{
    size_t len = strlen(s);
    char *new = malloc(len + 1);
    if (new)
    {
        memcpy(new, s, len + 1);
    }
    return new;
}

char *my_strndup(const char *s, size_t n)
{
    char *new = malloc(n + 1);
    if (new)
    {
        memcpy(new, s, n);
        new[n] = '\0';
    }
    return new;
}

int my_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *my_strtok(char *str, const char *delim)
{
    static char *last = NULL;
    if (str)
        last = str;
    if (!last)
        return NULL;

    // Skip leading delimiters
    while (*last && strchr(delim, *last))
        last++;

    if (!*last)
        return NULL;

    char *start = last;
    int in_quotes = 0;
    char quote_char = 0;

    while (*last)
    {
        if (*last == '\\')
        {
            last++;     // Skip the backslash
            if (*last)  // If there's a character after the backslash
                last++; // Skip the escaped character
            continue;
        }

        if (*last == '\'' || *last == '"')
        {
            if (!in_quotes)
            {
                in_quotes = 1;
                quote_char = *last;
            }
            else if (*last == quote_char)
            {
                in_quotes = 0;
            }
            last++;
            continue;
        }

        if (!in_quotes && strchr(delim, *last))
            break;

        last++;
    }

    if (*last)
    {
        *last = '\0';
        last++;
    }
    else
    {
        last = NULL;
    }

    return start;
}

char *my_strchr(const char *s, int c)
{
    while (*s && *s != (char)c)
        s++;
    return (*s == (char)c) ? (char *)s : NULL;
}

char *my_strrchr(const char *s, int c)
{
    const char *last = NULL;
    while (*s)
    {
        if (*s == (char)c)
            last = s;
        s++;
    }
    return (char *)last;
}

char *my_strcat(char *dest, const char *src)
{
    char *d = dest;
    while (*d)
        d++;
    while ((*d++ = *src++))
        ;
    return dest;
}

char *my_strncat(char *dest, const char *src, size_t n)
{
    char *d = dest;
    while (*d)
        d++;
    while (n-- && (*d++ = *src++))
        ;
    *d = '\0';
    return dest;
}

char *my_strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++))
        ;
    return dest;
}

char *my_strncpy(char *dest, const char *src, size_t n)
{
    char *d = dest;
    while (n-- && (*d++ = *src++))
        ;
    while (n--)
        *d++ = '\0';
    return dest;
}

int my_strncmp(const char *s1, const char *s2, size_t n)
{
    while (n-- && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return n == (size_t)-1 ? 0 : *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *my_strstr(const char *haystack, const char *needle)
{
    if (!*needle)
        return (char *)haystack;

    for (; *haystack; haystack++)
    {
        const char *h = haystack;
        const char *n = needle;
        while (*h && *n && *h == *n)
        {
            h++;
            n++;
        }
        if (!*n)
            return (char *)haystack;
    }
    return NULL;
}

void *my_memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    while (n--)
        *p++ = (unsigned char)c;
    return s;
}

void *my_memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--)
        *d++ = *s++;
    return dest;
}

void *my_memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    if (d < s)
    {
        while (n--)
            *d++ = *s++;
    }
    else
    {
        d += n;
        s += n;
        while (n--)
            *--d = *--s;
    }
    return dest;
}

int my_memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1;
    const unsigned char *p2 = s2;
    while (n--)
    {
        if (*p1 != *p2)
            return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}
