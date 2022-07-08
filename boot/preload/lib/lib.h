/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIB_H_
#define _LIB_H_

#ifndef __ASSEMBLY__

#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

extern int memtest_fast(size_t *addr, size_t size);
extern int memtest(size_t *addr, size_t size);

extern int vsprintf(char *, const char *, va_list);
extern void pr_init(void (*fun)(const char *));
extern int printf(const char *str, ...);
extern int pr_boot(const char *str, ...);
extern __noreturn void panic(const char *str, ...);

#endif  /* __ASSEMBLY__ */
#endif  /* _LIB_H_ */
