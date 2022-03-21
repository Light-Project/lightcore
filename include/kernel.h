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
#include <range.h>
#include <align.h>
#include <container.h>
#include <math.h>
#include <log2.h>

#define KMAGIC  0x4c434d4bU
#define MSIZE   (sizeof(void *))

#define REPEAT_BYTE(char) (                             \
    (~0UL / 0xff) * char                                \
)

#define COND_PTR(cond, ptr) (                           \
    (cond) ? (ptr) : NULL                               \
)

/**
 * ARRAY_SIZE - get the number of elements in array.
 * @arr: array to be sized.
 */
#define ARRAY_SIZE(arr) (                               \
    sizeof(arr) / sizeof((arr)[0])                      \
)

/**
 * swap - swap values
 * @a: first value
 * @b: second value
 */
#define swap(a, b) ({                                   \
    typeof(a) _a = (a);                                 \
    (a) = (b);                                          \
    (b) = _a;                                           \
})

/**
 * skzalloc - static alloc zeroed memory in stack
 * @size: memory size to alloc
 */
#define skzalloc(size) (                                \
    (void *)(char [size]){}                             \
)

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
    (u32)((val) & 0xffffffff)                           \
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

extern int atoi(const char *nptr);
extern unsigned long strtoul(const char *nptr);
extern unsigned long long strtoull(const char *nptr);

extern __printf(2, 3) int sprintf(char *buf, const char *fmt, ...);
extern __printf(3, 4) int snprintf(char *buf, size_t n, const char *fmt, ...);
extern __printf(3, 4) int scnprintf(char *buf, size_t n, const char *fmt, ...);
extern int vsprintf(char *buf, const char *fmt, va_list args);
extern int vsnprintf(char *buf, size_t n, const char *fmt, va_list args);
extern int vscnprintf(char *buf, size_t n, const char *fmt, va_list args);
extern int gsize(char *buff, size_t size);

#endif  /* _KERNEL_H_ */
