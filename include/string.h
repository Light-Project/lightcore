/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>
#include <stddef.h>

/*
 * String manipulation
 */
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
size_t strlcpy(char *dest, const char *src, size_t n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
size_t strlcat(char *dest, const char *src, size_t n);

/*
 * String examination
 */
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
int strnicmp(const char *s1, const char *s2, size_t n);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
int strcoll(const char *s1, const char *s2);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strnchr(const char *s, size_t n, int c);
char *strchrnul(const char *s, int c);
char *strnchrnul(const char *s, size_t count, int c);
size_t strspn(const char *s, const char *accept);
size_t strcspn(const char *s, const char *reject);
char *strpbrk(const char *s1, const char *s2);
char *strstr(const char *s1, const char *s2);
char *strnstr(const char *s1, const char *s2, size_t n);
char *strdup(const char *s);
char *strsep(char **s, const char *ct);
char *strim(char *s);

/*
 * Memory manipulation
 */
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void*src, size_t len);
void *memmove(void *dest, const void *src, size_t n);
void *memchr(const void *s, int c, size_t n);
void *memscan(void *addr, int c, size_t size);
int memcmp(const void *s1, const void *s2, size_t n);

static inline int memcount(const void *addr, int c, size_t size)
{
    const char *p = addr;
    int count;

    while (size--)
        if (c == *p++)
            count++;

    return count;
}

static inline const char *basename(const char *path)
{
    const char *c = strrchr(path, '/');
    return c ? c + 1 : path;
}

#endif /* _STRING_H_ */
