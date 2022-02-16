/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIB_H_
#define _LIB_H_

#include <types.h>
#include <stddef.h>
#include <stdarg.h>
#include <asm-generic/header.h>

extern void (*stdout)(const char *);

extern size_t strlen(const char *s);
extern size_t strnlen(const char *s, size_t n);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern char *strrchr(const char *s, int c);

extern void *memcpy(void *dest, const void* src, size_t len);
extern void *memset(void * s, int c, size_t n);
extern void *memmove(void * dest, const void * src, size_t n);
extern void *memchr(const void *s, int c, size_t n);
extern int memcmp(const void *s1, const void *s2, size_t n);

extern void heap_init(void *heap_start,size_t heap_size);
extern void *malloc(size_t size);
extern void free(void *mem);
extern int memtest_fast(size_t *addr, size_t size);
extern int memtest(size_t *addr, size_t size);

extern void kernel_check(void *addr);
extern void extract_kernel(void *output_start, void *input_start, unsigned long len);

extern int vsprintf(char *, const char *, va_list);
extern void pr_init(void (*fun)(const char *));
extern int pr_boot(const char *str,...);
extern void __noreturn panic(const char *str,...);

#endif  /* _LIB_H_ */
