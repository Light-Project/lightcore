/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIB_H_
#define _LIB_H_

#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <asm-generic/header.h>

extern void __malloc *heap_alloc(size_t size);
extern void heap_free(void *block);
extern void heap_setup(void *array, size_t size);

extern int memtest_fast(size_t *addr, size_t size);
extern int memtest(size_t *addr, size_t size);

extern void kernel_check(void *addr);
extern void extract_kernel(void *output_start, void *input_start, unsigned long len);

extern int vsprintf(char *buf, const char *fmt, va_list args);
extern void pr_init(void (*fun)(const char *buf));
extern int printf(const char *str, ...);
extern int pr_boot(const char *str, ...);
extern void __noreturn panic(const char *str, ...);

#endif  /* _LIB_H_ */
