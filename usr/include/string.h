#ifndef __STRING_H__
#define __STRING_H__

#include <stdint.h>
#include <stddef.h>
#include <ctype.h>

/*
 * String manipulation
 */
extern char * strcpy(char * dest, const char * src);
extern char * strncpy(char * dest, const char * src, size_t n);
extern size_t strlcpy(char * dest, const char * src, size_t n);
extern char * strcat(char * dest, const char * src);
extern char * strncat(char * dest, const char * src, size_t n);
extern size_t strlcat(char * dest, const char * src, size_t n);

/*
 * String examination
 */
extern size_t strlen(const char * s);
extern size_t strnlen(const char * s, size_t n);
extern int strcmp(const char * s1, const char * s2);
extern int strncmp(const char * s1, const char * s2, size_t n);
extern int strnicmp(const char * s1, const char * s2, size_t n);
extern int strcasecmp(const char * s1, const char * s2);
extern int strncasecmp(const char * s1, const char * s2, size_t n);
extern int strcoll(const char * s1, const char * s2);
extern char * strchr(const char * s, int c);
extern char * strrchr(const char * s, int c);
extern char * strnchr(const char * s, size_t n, int c);
extern size_t strspn(const char * s, const char * accept);
extern size_t strcspn(const char * s, const char * reject);
extern char * strpbrk(const char * s1, const char * s2);
extern char * strstr(const char * s1, const char * s2);
extern char * strnstr(const char * s1, const char * s2, size_t n);
extern char * strdup(const char * s);
extern char * strsep(char ** s, const char * ct);
extern char * strim(char * s);

/*
 * Memory manipulation
 */
extern void * memset(void * s, int c, size_t n);
extern void * memcpy(void * dest, const void * src, size_t len);
extern void * memmove(void * dest, const void * src, size_t n);
extern void * memchr(const void * s, int c, size_t n);
extern void * memscan(void * addr, int c, size_t size);
extern int memcmp(const void * s1, const void * s2, size_t n);

#endif /* __STRING_H__ */
