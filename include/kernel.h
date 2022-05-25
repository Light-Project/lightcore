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

extern int strntoi(const char *nptr, char **endptr, unsigned int base, size_t len);
extern long strntol(const char *nptr, char **endptr, unsigned int base, size_t len);
extern long long strntoll(const char *nptr, char **endptr, unsigned int base, size_t len);
extern intmax_t strntoimax(const char *nptr, char **endptr, unsigned int base, size_t len);
extern unsigned int strntoui(const char *nptr, char **endptr, unsigned int base, size_t len);
extern unsigned long strntoul(const char *nptr, char **endptr, unsigned int base, size_t len);
extern unsigned long long strntoull(const char *nptr, char **endptr, unsigned int base, size_t len);
extern uintmax_t strntoumax(const char *nptr, char **endptr, unsigned int base, size_t len);

extern int8_t strntos8(const char *nptr, char **endptr, unsigned int base, size_t len);
extern int16_t strntos16(const char *nptr, char **endptr, unsigned int base, size_t len);
extern int32_t strntos32(const char *nptr, char **endptr, unsigned int base, size_t len);
extern int64_t strntos64(const char *nptr, char **endptr, unsigned int base, size_t len);
extern uint8_t strntou8(const char *nptr, char **endptr, unsigned int base, size_t len);
extern uint16_t strntou16(const char *nptr, char **endptr, unsigned int base, size_t len);
extern uint32_t strntou32(const char *nptr, char **endptr, unsigned int base, size_t len);
extern uint64_t strntou64(const char *nptr, char **endptr, unsigned int base, size_t len);

extern int strtoi(const char *nptr, char **endptr, unsigned int base);
extern long strtol(const char *nptr, char **endptr, unsigned int base);
extern long long strtoll(const char *nptr, char **endptr, unsigned int base);
extern intmax_t strtoimax(const char *nptr, char **endptr, unsigned int base);
extern unsigned int strtoui(const char *nptr, char **endptr, unsigned int base);
extern unsigned long strtoul(const char *nptr, char **endptr, unsigned int base);
extern unsigned long long strtoull(const char *nptr, char **endptr, unsigned int base);
extern uintmax_t strtoumax(const char *nptr, char **endptr, unsigned int base);

extern int8_t strtos8(const char *nptr, char **endptr, unsigned int base);
extern int16_t strtos16(const char *nptr, char **endptr, unsigned int base);
extern int32_t strtos32(const char *nptr, char **endptr, unsigned int base);
extern int64_t strtos64(const char *nptr, char **endptr, unsigned int base);
extern uint8_t strtou8(const char *nptr, char **endptr, unsigned int base);
extern uint16_t strtou16(const char *nptr, char **endptr, unsigned int base);
extern uint32_t strtou32(const char *nptr, char **endptr, unsigned int base);
extern uint64_t strtou64(const char *nptr, char **endptr, unsigned int base);

extern int axtoi(const char *nptr);
extern long axtol(const char *nptr);
extern long long axtoll(const char *nptr);
extern intmax_t axtoimax(const char *nptr);
extern unsigned int axtoui(const char *nptr);
extern unsigned long axtoul(const char *nptr);
extern unsigned long long axtoull(const char *nptr);
extern uintmax_t axtoumax(const char *nptr);

extern int8_t axtos8(const char *nptr);
extern int16_t axtos16(const char *nptr);
extern int32_t axtos32(const char *nptr);
extern int64_t axtos64(const char *nptr);
extern uint8_t axtou8(const char *nptr);
extern uint16_t axtou16(const char *nptr);
extern uint32_t axtou32(const char *nptr);
extern uint64_t axtou64(const char *nptr);

extern int atoi(const char *nptr);
extern long atol(const char *nptr);
extern long long atoll(const char *nptr);
extern intmax_t atoimax(const char *nptr);
extern unsigned int atoui(const char *nptr);
extern unsigned long atoul(const char *nptr);
extern unsigned long long atoull(const char *nptr);
extern uintmax_t atoumax(const char *nptr);

extern int8_t atos8(const char *nptr);
extern int16_t atos16(const char *nptr);
extern int32_t atos32(const char *nptr);
extern int64_t atos64(const char *nptr);
extern uint8_t atou8(const char *nptr);
extern uint16_t atou16(const char *nptr);
extern uint32_t atou32(const char *nptr);
extern uint64_t atou64(const char *nptr);

extern char *itoa(int val, char *str, int base);
extern __scanf(2, 0) int vsscanf(const char *buf, const char *fmt, va_list args);
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
