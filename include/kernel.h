/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <stdlib.h>
#include <error.h>
#include <state.h>

#include <math.h>
#include <log2.h>
#include <size.h>

#include <macro.h>
#include <alloca.h>
#include <minmax.h>
#include <align.h>
#include <container.h>
#include <lightcore/kernel.h>

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

extern int chtoi(unsigned char ch);
extern __scanf(2, 0) int vsscanf(const char *buf, const char *fmt, va_list args);
extern __scanf(2, 3) int sscanf(const char *buf, const char *fmt, ...);
extern __printf(2, 3) int sprintf(char *buf, const char *fmt, ...);
extern __printf(3, 4) int snprintf(char *buf, size_t size, const char *fmt, ...);
extern __printf(3, 4) int scnprintf(char *buf, size_t size, const char *fmt, ...);
extern __printf(2, 0) int vsprintf(char *buf, const char *fmt, va_list args);
extern __printf(3, 0) int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
extern __printf(3, 0) int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
extern __printf(2, 0) __malloc char *kvasprintf(gfp_t flags, const char *fmt, va_list args);
extern __printf(2, 0) __malloc const char *kvasprintf_const(gfp_t flags, const char *fmt, va_list args);
extern __printf(2, 3) __malloc char *kasprintf(gfp_t gfp, const char *fmt, ...);
extern __printf(2, 3) __malloc const char *kasprintf_const(gfp_t gfp, const char *fmt, ...);

#endif  /* _KERNEL_H_ */
