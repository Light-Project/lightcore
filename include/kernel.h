/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <types.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <error.h>
#include <state.h>
#include <size.h>
#include <macro.h>
#include <range.h>
#include <align.h>
#include <container.h>
#include <math.h>
#include <log2.h>

#define KMAGIC  0x4c434d4bU
#define MSIZE   (sizeof(void *))

/**
 * upper_32_bits - return bits 32-63 of a number
 * @val: the value we're accessing
 */
#define upper_32_bits(val) (                            \
    (uint32_t)(((val) >> 16) >> 16)                     \
)

/**
 * lower_32_bits - return bits 0-31 of a number
 * @val: the value we're accessing
 */
#define lower_32_bits(val) (                            \
    (uint32_t)((val) & 0xffffffff)                      \
)

/**
 * upper_16_bits - return bits 16-31 of a number
 * @val: the value we're accessing
 */
#define upper_16_bits(val) (                            \
    (uint16_t)((val) >> 16)                             \
)

/**
 * lower_16_bits - return bits 0-15 of a number
 * @val: the value we're accessing
 */
#define lower_16_bits(val) (                            \
    (uint16_t)((val) & 0xffff)                          \
)

extern char *itoa(int val, char *str, int base);
extern int atoi(const char *nptr);

extern int strntoi(const char *nptr, unsigned int len);
extern long strntol(const char *nptr, unsigned int len);
extern long long strntoll(const char *nptr, unsigned int len);
extern unsigned int strntoui(const char *nptr, unsigned int len);
extern unsigned long strntoul(const char *nptr, unsigned int len);
extern unsigned long long strntoull(const char *nptr, unsigned int len);
extern int strtoi(const char *nptr);
extern long strtol(const char *nptr);
extern long long strtoll(const char *nptr);
extern unsigned int strtoui(const char *nptr);
extern unsigned long strtoul(const char *nptr);
extern unsigned long long strtoull(const char *nptr);

extern __printf(2, 3) int sprintf(char *buf, const char *fmt, ...);
extern __printf(3, 4) int snprintf(char *buf, size_t n, const char *fmt, ...);
extern __printf(3, 4) int scnprintf(char *buf, size_t n, const char *fmt, ...);
extern __printf(2, 0) int vsprintf(char *buf, const char *fmt, va_list args);
extern __printf(3, 0) int vsnprintf(char *buf, size_t n, const char *fmt, va_list args);
extern __printf(3, 0) int vscnprintf(char *buf, size_t n, const char *fmt, va_list args);
extern __printf(2, 0) __malloc char *kvasprintf(gfp_t flags, const char *fmt, va_list args);
extern __printf(2, 3) __malloc char *kasprintf(gfp_t gfp, const char *fmt, ...);
extern int gsize(char *buff, size_t size);

#endif  /* _KERNEL_H_ */
