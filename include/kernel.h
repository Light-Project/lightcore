/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <types.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <error.h>
#include <state.h>

#include <math.h>
#include <log2.h>
#include <size.h>

#include <macro.h>
#include <range.h>
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

extern int intoa(int val, char *buff, int base, size_t length);
extern int lntoa(long val, char *buff, int base, size_t length);
extern int llntoa(long long val, char *buff, int base, size_t length);
extern int imaxntoa(intmax_t val, char *buff, int base, size_t length);
extern int uintoa(unsigned int val, char *buff, int base, size_t length);
extern int ulntoa(unsigned long val, char *buff, int base, size_t length);
extern int ullntoa(unsigned long long val, char *buff, int base, size_t length);
extern int umaxntoa(uintmax_t val, char *buff, int base, size_t length);
extern int s8ntoa(int8_t val, char *buff, int base, size_t length);
extern int s16ntoa(int16_t val, char *buff, int base, size_t length);
extern int s32ntoa(int32_t val, char *buff, int base, size_t length);
extern int s64ntoa(int64_t val, char *buff, int base, size_t length);
extern int u8ntoa(uint8_t val, char *buff, int base, size_t length);
extern int u16ntoa(uint16_t val, char *buff, int base, size_t length);
extern int u32ntoa(uint32_t val, char *buff, int base, size_t length);
extern int u64ntoa(uint64_t val, char *buff, int base, size_t length);

extern int itoa(int val, char *buff, int base);
extern int ltoa(long val, char *buff, int base);
extern int lltoa(long long val, char *buff, int base);
extern int imaxtoa(intmax_t val, char *buff, int base);
extern int uitoa(unsigned int val, char *buff, int base);
extern int ultoa(unsigned long val, char *buff, int base);
extern int ulltoa(unsigned long long val, char *buff, int base);
extern int umaxtoa(uintmax_t val, char *buff, int base);
extern int s8toa(int8_t val, char *buff, int base);
extern int s16toa(int16_t val, char *buff, int base);
extern int s32toa(int32_t val, char *buff, int base);
extern int s64toa(int64_t val, char *buff, int base);
extern int u8toa(uint8_t val, char *buff, int base);
extern int u16toa(uint16_t val, char *buff, int base);
extern int u32toa(uint32_t val, char *buff, int base);
extern int u64toa(uint64_t val, char *buff, int base);

extern int itoad(int val, char *buff);
extern int ltoad(long val, char *buff);
extern int lltoad(long long val, char *buff);
extern int imaxtoad(intmax_t val, char *buff);
extern int uitoad(unsigned int val, char *buff);
extern int ultoad(unsigned long val, char *buff);
extern int ulltoad(unsigned long long val, char *buff);
extern int umaxtoad(uintmax_t val, char *buff);
extern int s8toad(int8_t val, char *buff);
extern int s16toad(int16_t val, char *buff);
extern int s32toad(int32_t val, char *buff);
extern int s64toad(int64_t val, char *buff);
extern int u8toad(uint8_t val, char *buff);
extern int u16toad(uint16_t val, char *buff);
extern int u32toad(uint32_t val, char *buff);
extern int u64toad(uint64_t val, char *buff);

extern int itoab(int val, char *buff);
extern int ltoab(long val, char *buff);
extern int lltoab(long long val, char *buff);
extern int imaxtoab(intmax_t val, char *buff);
extern int uitoab(unsigned int val, char *buff);
extern int ultoab(unsigned long val, char *buff);
extern int ulltoab(unsigned long long val, char *buff);
extern int umaxtoab(uintmax_t val, char *buff);
extern int s8toab(int8_t val, char *buff);
extern int s16toab(int16_t val, char *buff);
extern int s32toab(int32_t val, char *buff);
extern int s64toab(int64_t val, char *buff);
extern int u8toab(uint8_t val, char *buff);
extern int u16toab(uint16_t val, char *buff);
extern int u32toab(uint32_t val, char *buff);
extern int u64toab(uint64_t val, char *buff);

extern int itoao(int val, char *buff);
extern int ltoao(long val, char *buff);
extern int lltoao(long long val, char *buff);
extern int imaxtoao(intmax_t val, char *buff);
extern int uitoao(unsigned int val, char *buff);
extern int ultoao(unsigned long val, char *buff);
extern int ulltoao(unsigned long long val, char *buff);
extern int umaxtoao(uintmax_t val, char *buff);
extern int s8toao(int8_t val, char *buff);
extern int s16toao(int16_t val, char *buff);
extern int s32toao(int32_t val, char *buff);
extern int s64toao(int64_t val, char *buff);
extern int u8toao(uint8_t val, char *buff);
extern int u16toao(uint16_t val, char *buff);
extern int u32toao(uint32_t val, char *buff);
extern int u64toao(uint64_t val, char *buff);

extern int itoah(int val, char *buff);
extern int ltoah(long val, char *buff);
extern int lltoah(long long val, char *buff);
extern int imaxtoah(intmax_t val, char *buff);
extern int uitoah(unsigned int val, char *buff);
extern int ultoah(unsigned long val, char *buff);
extern int ulltoah(unsigned long long val, char *buff);
extern int umaxtoah(uintmax_t val, char *buff);
extern int s8toah(int8_t val, char *buff);
extern int s16toah(int16_t val, char *buff);
extern int s32toah(int32_t val, char *buff);
extern int s64toah(int64_t val, char *buff);
extern int u8toah(uint8_t val, char *buff);
extern int u16toah(uint16_t val, char *buff);
extern int u32toah(uint32_t val, char *buff);
extern int u64toah(uint64_t val, char *buff);

extern int strntoi(const char *nptr, char **endptr, unsigned int base, size_t length);
extern long strntol(const char *nptr, char **endptr, unsigned int base, size_t length);
extern long long strntoll(const char *nptr, char **endptr, unsigned int base, size_t length);
extern intmax_t strntoimax(const char *nptr, char **endptr, unsigned int base, size_t length);
extern unsigned int strntoui(const char *nptr, char **endptr, unsigned int base, size_t length);
extern unsigned long strntoul(const char *nptr, char **endptr, unsigned int base, size_t length);
extern unsigned long long strntoull(const char *nptr, char **endptr, unsigned int base, size_t length);
extern uintmax_t strntoumax(const char *nptr, char **endptr, unsigned int base, size_t length);
extern int8_t strntos8(const char *nptr, char **endptr, unsigned int base, size_t length);
extern int16_t strntos16(const char *nptr, char **endptr, unsigned int base, size_t length);
extern int32_t strntos32(const char *nptr, char **endptr, unsigned int base, size_t length);
extern int64_t strntos64(const char *nptr, char **endptr, unsigned int base, size_t length);
extern uint8_t strntou8(const char *nptr, char **endptr, unsigned int base, size_t length);
extern uint16_t strntou16(const char *nptr, char **endptr, unsigned int base, size_t length);
extern uint32_t strntou32(const char *nptr, char **endptr, unsigned int base, size_t length);
extern uint64_t strntou64(const char *nptr, char **endptr, unsigned int base, size_t length);

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

extern int abtoi(const char *nptr);
extern long abtol(const char *nptr);
extern long long abtoll(const char *nptr);
extern intmax_t abtoimax(const char *nptr);
extern unsigned int abtoui(const char *nptr);
extern unsigned long abtoul(const char *nptr);
extern unsigned long long abtoull(const char *nptr);
extern uintmax_t abtoumax(const char *nptr);
extern int8_t abtos8(const char *nptr);
extern int16_t abtos16(const char *nptr);
extern int32_t abtos32(const char *nptr);
extern int64_t abtos64(const char *nptr);
extern uint8_t abtou8(const char *nptr);
extern uint16_t abtou16(const char *nptr);
extern uint32_t abtou32(const char *nptr);
extern uint64_t abtou64(const char *nptr);

extern int aotoi(const char *nptr);
extern long aotol(const char *nptr);
extern long long aotoll(const char *nptr);
extern intmax_t aotoimax(const char *nptr);
extern unsigned int aotoui(const char *nptr);
extern unsigned long aotoul(const char *nptr);
extern unsigned long long aotoull(const char *nptr);
extern uintmax_t aotoumax(const char *nptr);
extern int8_t aotos8(const char *nptr);
extern int16_t aotos16(const char *nptr);
extern int32_t aotos32(const char *nptr);
extern int64_t aotos64(const char *nptr);
extern uint8_t aotou8(const char *nptr);
extern uint16_t aotou16(const char *nptr);
extern uint32_t aotou32(const char *nptr);
extern uint64_t aotou64(const char *nptr);

extern int ahtoi(const char *nptr);
extern long ahtol(const char *nptr);
extern long long ahtoll(const char *nptr);
extern intmax_t ahtoimax(const char *nptr);
extern unsigned int ahtoui(const char *nptr);
extern unsigned long ahtoul(const char *nptr);
extern unsigned long long ahtoull(const char *nptr);
extern uintmax_t ahtoumax(const char *nptr);
extern int8_t ahtos8(const char *nptr);
extern int16_t ahtos16(const char *nptr);
extern int32_t ahtos32(const char *nptr);
extern int64_t ahtos64(const char *nptr);
extern uint8_t ahtou8(const char *nptr);
extern uint16_t ahtou16(const char *nptr);
extern uint32_t ahtou32(const char *nptr);
extern uint64_t ahtou64(const char *nptr);

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
