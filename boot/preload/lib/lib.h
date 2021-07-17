#ifndef _LIB_H_
#define _LIB_H_
#ifndef __ASSEMBLY__

#include <types.h>
#include <stddef.h>
#include <stdarg.h>

extern void cmd_init(void);

extern void pr_init(void (*fun)(const char *));
extern int printf(const char *str, ...);
extern int pr_boot(const char *str, ...);
extern void panic(const char *str, ...);

extern int strcmp(const char *, const char *);
extern void *memcpy(void *, const void*, size_t);
extern void *memset(void *, int, size_t);
extern void *memmove(void * dest, const void * src, size_t n);

extern int vsprintf(char *, const char *, va_list);

#endif

#endif
