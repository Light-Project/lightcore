/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>
#include <stddef.h>

/*
 * String manipulation
 */
extern char *strcpy(char *dest, const char *src);
extern char *strncpy(char *dest, const char *src, size_t n);
extern size_t strlcpy(char *dest, const char *src, size_t n);
extern char *strcat(char *dest, const char *src);
extern char *strncat(char *dest, const char *src, size_t n);
extern size_t strlcat(char *dest, const char *src, size_t n);

/*
 * String examination
 */
extern size_t strlen(const char *s);
extern size_t strnlen(const char *s, size_t n);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern int strnicmp(const char *s1, const char *s2, size_t n);
extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
extern int strcoll(const char *s1, const char *s2);
extern char *strchr(const char *s, int c);
extern char *strrchr(const char *s, int c);
extern char *strnchr(const char *s, size_t n, int c);
extern char *strrnchr(const char *s, size_t n, int c);
extern char *strchrnul(const char *s, int c);
extern char *strnchrnul(const char *s, size_t count, int c);
extern size_t strspn(const char *s, const char *accept);
extern size_t strcspn(const char *s, const char *reject);
extern char *strpbrk(const char *s1, const char *s2);
extern char *strstr(const char *s1, const char *s2);
extern char *strnstr(const char *s1, const char *s2, size_t n);
extern char *strdup(const char *s);
extern char *strsep(char **s, const char *ct);
extern char *strim(char *s);

/*
 * Memory manipulation
 */
extern void *memset(void *s, int c, size_t n);
extern void *memcpy(void *dest, const void*src, size_t len);
extern void *memmove(void *dest, const void *src, size_t n);
extern void *memchr(const void *s, int c, size_t n);
extern void *memscan(void *addr, int c, size_t size);
extern int memcmp(const void *s1, const void *s2, size_t n);

/*
 * Extend Helper
 */
extern int strcount(const char *str, char c);
extern int strncount(const char *str, char c, size_t size);
extern int memcount(const void *addr, long c, size_t size);
extern void *memdiff(const void *addr, long c, size_t size);
extern char *skip_spaces(const char *path);
extern char *basename(const char *path);

#endif /* _STRING_H_ */
