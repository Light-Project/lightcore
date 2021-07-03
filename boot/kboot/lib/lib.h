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

/* lib/check.c */
void kernel_check(void *addr);

/* lib/mem.c */
void heap_init(void *heap_start,size_t heap_size);
void *malloc(size_t size);
void free(void *mem);

void memtest(void *mem_start, size_t size);

void extract_kernel(void *output_start, void *input_start, unsigned long len);

void pr_init(void (*fun)(const char *));
int pr_boot(const char *str,...);
void panic(const char *str,...);

int strcmp(const char *, const char *);
void *memset(void * s, int c, size_t n);
void *memcpy(void *dest, const void* src, size_t len);

int vsprintf(char *, const char *, va_list);

#endif /* _LIB_H_ */
