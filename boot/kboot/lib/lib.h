/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIB_H_
#define _LIB_H_

#include <types.h>
#include <stddef.h>
#include <stdarg.h>
#include <asm-generic/header.h>

extern char _ld_piggy_start;
extern char _ld_piggy_end;
extern char _ld_bss_start;
extern char _ld_bss_end;
extern char _ld_heap_start;
extern char _ld_heap_end;

extern void *ld_piggy_start;
extern void *ld_piggy_end;
extern void *ld_bss_start;
extern void *ld_bss_end;
extern void *ld_heap_start;
extern void *ld_heap_end;

extern void (*stdout)(const char *);

size_t strlen(const char *s);
size_t strnlen(const char *s, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strrchr(const char *s, int c);

void *memcpy(void *dest, const void* src, size_t len);
void *memset(void * s, int c, size_t n);
void *memmove(void * dest, const void * src, size_t n);
void *memchr(const void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

void heap_init(void *heap_start,size_t heap_size);
void *malloc(size_t size);
void free(void *mem);
int memtest_fast(size_t *addr, size_t size);
int memtest(size_t *addr, size_t size);

void kernel_check(void *addr);
void extract_kernel(void *output_start, void *input_start, unsigned long len);

int vsprintf(char *, const char *, va_list);
void pr_init(void (*fun)(const char *));
int pr_boot(const char *str,...);
void panic(const char *str,...);

#endif  /* _LIB_H_ */
