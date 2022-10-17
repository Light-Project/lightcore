/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <types.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>

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

#define GENERIC_XBTOA_OPS(name, func, type) \
static __always_inline int                  \
name(type val, char *str, int base)         \
{                                           \
    return func(val, str, base, ULONG_MAX); \
}

GENERIC_XBTOA_OPS(itoa, intoa, int)
GENERIC_XBTOA_OPS(ltoa, lntoa, long)
GENERIC_XBTOA_OPS(lltoa, llntoa, long long)
GENERIC_XBTOA_OPS(imaxtoa, imaxntoa, intmax_t)
GENERIC_XBTOA_OPS(uitoa, uintoa, unsigned int)
GENERIC_XBTOA_OPS(ultoa, ulntoa, unsigned long)
GENERIC_XBTOA_OPS(ulltoa, ullntoa, unsigned long long)
GENERIC_XBTOA_OPS(umaxtoa, umaxntoa, uintmax_t)
GENERIC_XBTOA_OPS(s8toa, s8ntoa, int8_t)
GENERIC_XBTOA_OPS(s16toa, s16ntoa, int16_t)
GENERIC_XBTOA_OPS(s32toa, s32ntoa, int32_t)
GENERIC_XBTOA_OPS(s64toa, s64ntoa, int64_t)
GENERIC_XBTOA_OPS(u8toa, u8ntoa, uint8_t)
GENERIC_XBTOA_OPS(u16toa, u16ntoa, uint16_t)
GENERIC_XBTOA_OPS(u32toa, u32ntoa, uint32_t)
GENERIC_XBTOA_OPS(u64toa, u64ntoa, uint64_t)

#define GENERIC_XTOA_OPS(name, func, type, base)    \
static __always_inline int                          \
name(type val, char *str)                           \
{                                                   \
    return func(val, str, base);                    \
}

GENERIC_XTOA_OPS(itoad, itoa, int, 10)
GENERIC_XTOA_OPS(ltoad, ltoa, long, 10)
GENERIC_XTOA_OPS(lltoad, lltoa, long long, 10)
GENERIC_XTOA_OPS(imaxtoad, imaxtoa, intmax_t, 10)
GENERIC_XTOA_OPS(uitoad, uitoa, unsigned int, 10)
GENERIC_XTOA_OPS(ultoad, ultoa, unsigned long, 10)
GENERIC_XTOA_OPS(ulltoad, ulltoa, unsigned long long, 10)
GENERIC_XTOA_OPS(umaxtoad, umaxtoa, uintmax_t, 10)
GENERIC_XTOA_OPS(s8toad, s8toa, int8_t, 10)
GENERIC_XTOA_OPS(s16toad, s16toa, int16_t, 10)
GENERIC_XTOA_OPS(s32toad, s32toa, int32_t, 10)
GENERIC_XTOA_OPS(s64toad, s64toa, int64_t, 10)
GENERIC_XTOA_OPS(u8toad, u8toa, uint8_t, 10)
GENERIC_XTOA_OPS(u16toad, u16toa, uint16_t, 10)
GENERIC_XTOA_OPS(u32toad, u32toa, uint32_t, 10)
GENERIC_XTOA_OPS(u64toad, u64toa, uint64_t, 10)

GENERIC_XTOA_OPS(itoab, itoa, int, 2)
GENERIC_XTOA_OPS(ltoab, ltoa, long, 2)
GENERIC_XTOA_OPS(lltoab, lltoa, long long, 2)
GENERIC_XTOA_OPS(imaxtoab, imaxtoa, intmax_t, 2)
GENERIC_XTOA_OPS(uitoab, uitoa, unsigned int, 2)
GENERIC_XTOA_OPS(ultoab, ultoa, unsigned long, 2)
GENERIC_XTOA_OPS(ulltoab, ulltoa, unsigned long long, 2)
GENERIC_XTOA_OPS(umaxtoab, umaxtoa, uintmax_t, 2)
GENERIC_XTOA_OPS(s8toab, s8toa, int8_t, 2)
GENERIC_XTOA_OPS(s16toab, s16toa, int16_t, 2)
GENERIC_XTOA_OPS(s32toab, s32toa, int32_t, 2)
GENERIC_XTOA_OPS(s64toab, s64toa, int64_t, 2)
GENERIC_XTOA_OPS(u8toab, u8toa, uint8_t, 2)
GENERIC_XTOA_OPS(u16toab, u16toa, uint16_t, 2)
GENERIC_XTOA_OPS(u32toab, u32toa, uint32_t, 2)
GENERIC_XTOA_OPS(u64toab, u64toa, uint64_t, 2)

GENERIC_XTOA_OPS(itoao, itoa, int, 8)
GENERIC_XTOA_OPS(ltoao, ltoa, long, 8)
GENERIC_XTOA_OPS(lltoao, lltoa, long long, 8)
GENERIC_XTOA_OPS(imaxtoao, imaxtoa, intmax_t, 8)
GENERIC_XTOA_OPS(uitoao, uitoa, unsigned int, 8)
GENERIC_XTOA_OPS(ultoao, ultoa, unsigned long, 8)
GENERIC_XTOA_OPS(ulltoao, ulltoa, unsigned long long, 8)
GENERIC_XTOA_OPS(umaxtoao, umaxtoa, uintmax_t, 8)
GENERIC_XTOA_OPS(s8toao, s8toa, int8_t, 8)
GENERIC_XTOA_OPS(s16toao, s16toa, int16_t, 8)
GENERIC_XTOA_OPS(s32toao, s32toa, int32_t, 8)
GENERIC_XTOA_OPS(s64toao, s64toa, int64_t, 8)
GENERIC_XTOA_OPS(u8toao, u8toa, uint8_t, 8)
GENERIC_XTOA_OPS(u16toao, u16toa, uint16_t, 8)
GENERIC_XTOA_OPS(u32toao, u32toa, uint32_t, 8)
GENERIC_XTOA_OPS(u64toao, u64toa, uint64_t, 8)

GENERIC_XTOA_OPS(itoah, itoa, int, 16)
GENERIC_XTOA_OPS(ltoah, ltoa, long, 16)
GENERIC_XTOA_OPS(lltoah, lltoa, long long, 16)
GENERIC_XTOA_OPS(imaxtoah, imaxtoa, intmax_t, 16)
GENERIC_XTOA_OPS(uitoah, uitoa, unsigned int, 16)
GENERIC_XTOA_OPS(ultoah, ultoa, unsigned long, 16)
GENERIC_XTOA_OPS(ulltoah, ulltoa, unsigned long long, 16)
GENERIC_XTOA_OPS(umaxtoah, umaxtoa, uintmax_t, 16)
GENERIC_XTOA_OPS(s8toah, s8toa, int8_t, 16)
GENERIC_XTOA_OPS(s16toah, s16toa, int16_t, 16)
GENERIC_XTOA_OPS(s32toah, s32toa, int32_t, 16)
GENERIC_XTOA_OPS(s64toah, s64toa, int64_t, 16)
GENERIC_XTOA_OPS(u8toah, u8toa, uint8_t, 16)
GENERIC_XTOA_OPS(u16toah, u16toa, uint16_t, 16)
GENERIC_XTOA_OPS(u32toah, u32toa, uint32_t, 16)
GENERIC_XTOA_OPS(u64toah, u64toa, uint64_t, 16)

#define GENERIC_STRTOX_OPS(name, func, type)                \
static __always_inline type                                 \
name(const char *nptr, char **endptr, unsigned int base)    \
{                                                           \
    return func(nptr, endptr, base, ULONG_MAX);             \
}

GENERIC_STRTOX_OPS(strtoi, strntoi, int)
GENERIC_STRTOX_OPS(strtol, strntol, long)
GENERIC_STRTOX_OPS(strtoll, strntoll, long long)
GENERIC_STRTOX_OPS(strtoimax, strntoimax, intmax_t)
GENERIC_STRTOX_OPS(strtoui, strntoui, unsigned int)
GENERIC_STRTOX_OPS(strtoul, strntoul, unsigned long)
GENERIC_STRTOX_OPS(strtoull, strntoull, unsigned long long)
GENERIC_STRTOX_OPS(strtoumax, strntoumax, uintmax_t)
GENERIC_STRTOX_OPS(strtos8, strntos8, int8_t)
GENERIC_STRTOX_OPS(strtos16, strntos16, int16_t)
GENERIC_STRTOX_OPS(strtos32, strntos32, int32_t)
GENERIC_STRTOX_OPS(strtos64, strntos64, int64_t)
GENERIC_STRTOX_OPS(strtou8, strntou8, uint8_t)
GENERIC_STRTOX_OPS(strtou16, strntou16, uint16_t)
GENERIC_STRTOX_OPS(strtou32, strntou32, uint32_t)
GENERIC_STRTOX_OPS(strtou64, strntou64, uint64_t)

#define GENERIC_AXTOX_OPS(name, func, type) \
static __always_inline type                 \
name(const char *nptr)                      \
{                                           \
    return func(nptr, NULL, 0);             \
}

GENERIC_AXTOX_OPS(axtoi, strtoi, int)
GENERIC_AXTOX_OPS(axtol, strtol, long)
GENERIC_AXTOX_OPS(axtoll, strtoll, long long)
GENERIC_AXTOX_OPS(axtoimax, strtoimax, intmax_t)
GENERIC_AXTOX_OPS(axtoui, strtoui, unsigned int)
GENERIC_AXTOX_OPS(axtoul, strtoul, unsigned long)
GENERIC_AXTOX_OPS(axtoull, strtoull, unsigned long long)
GENERIC_AXTOX_OPS(axtoumax, strtoumax, uintmax_t)
GENERIC_AXTOX_OPS(axtos8, strtos8, int8_t)
GENERIC_AXTOX_OPS(axtos16, strtos16, int16_t)
GENERIC_AXTOX_OPS(axtos32, strtos32, int32_t)
GENERIC_AXTOX_OPS(axtos64, strtos64, int64_t)
GENERIC_AXTOX_OPS(axtou8, strtou8, uint8_t)
GENERIC_AXTOX_OPS(axtou16, strtou16, uint16_t)
GENERIC_AXTOX_OPS(axtou32, strtou32, uint32_t)
GENERIC_AXTOX_OPS(axtou64, strtou64, uint64_t)

#define GENERIC_ATOX_OPS(name, func, type, base)    \
static __always_inline type                         \
name(const char *nptr)                              \
{                                                   \
    return func(nptr, NULL, base);                  \
}

GENERIC_ATOX_OPS(atoi, strtoi, int, 10)
GENERIC_ATOX_OPS(atol, strtol, long, 10)
GENERIC_ATOX_OPS(atoll, strtoll, long long, 10)
GENERIC_ATOX_OPS(atoimax, strtoimax, intmax_t, 10)
GENERIC_ATOX_OPS(atoui, strtoui, unsigned int, 10)
GENERIC_ATOX_OPS(atoul, strtoul, unsigned long, 10)
GENERIC_ATOX_OPS(atoull, strtoull, unsigned long long, 10)
GENERIC_ATOX_OPS(atoumax, strtoumax, uintmax_t, 10)
GENERIC_ATOX_OPS(atos8, strtos8, int8_t, 10)
GENERIC_ATOX_OPS(atos16, strtos16, int16_t, 10)
GENERIC_ATOX_OPS(atos32, strtos32, int32_t, 10)
GENERIC_ATOX_OPS(atos64, strtos64, int64_t, 10)
GENERIC_ATOX_OPS(atou8, strtou8, uint8_t, 10)
GENERIC_ATOX_OPS(atou16, strtou16, uint16_t, 10)
GENERIC_ATOX_OPS(atou32, strtou32, uint32_t, 10)
GENERIC_ATOX_OPS(atou64, strtou64, uint64_t, 10)

GENERIC_ATOX_OPS(abtoi, strtoi, int, 2)
GENERIC_ATOX_OPS(abtol, strtol, long, 2)
GENERIC_ATOX_OPS(abtoll, strtoll, long long, 2)
GENERIC_ATOX_OPS(abtoimax, strtoimax, intmax_t, 2)
GENERIC_ATOX_OPS(abtoui, strtoui, unsigned int, 2)
GENERIC_ATOX_OPS(abtoul, strtoul, unsigned long, 2)
GENERIC_ATOX_OPS(abtoull, strtoull, unsigned long long, 2)
GENERIC_ATOX_OPS(abtoumax, strtoumax, uintmax_t, 2)
GENERIC_ATOX_OPS(abtos8, strtos8, int8_t, 2)
GENERIC_ATOX_OPS(abtos16, strtos16, int16_t, 2)
GENERIC_ATOX_OPS(abtos32, strtos32, int32_t, 2)
GENERIC_ATOX_OPS(abtos64, strtos64, int64_t, 2)
GENERIC_ATOX_OPS(abtou8, strtou8, uint8_t, 2)
GENERIC_ATOX_OPS(abtou16, strtou16, uint16_t, 2)
GENERIC_ATOX_OPS(abtou32, strtou32, uint32_t, 2)
GENERIC_ATOX_OPS(abtou64, strtou64, uint64_t, 2)

GENERIC_ATOX_OPS(aotoi, strtoi, int, 8)
GENERIC_ATOX_OPS(aotol, strtol, long, 8)
GENERIC_ATOX_OPS(aotoll, strtoll, long long, 8)
GENERIC_ATOX_OPS(aotoimax, strtoimax, intmax_t, 8)
GENERIC_ATOX_OPS(aotoui, strtoui, unsigned int, 8)
GENERIC_ATOX_OPS(aotoul, strtoul, unsigned long, 8)
GENERIC_ATOX_OPS(aotoull, strtoull, unsigned long long, 8)
GENERIC_ATOX_OPS(aotoumax, strtoumax, uintmax_t, 8)
GENERIC_ATOX_OPS(aotos8, strtos8, int8_t, 8)
GENERIC_ATOX_OPS(aotos16, strtos16, int16_t, 8)
GENERIC_ATOX_OPS(aotos32, strtos32, int32_t, 8)
GENERIC_ATOX_OPS(aotos64, strtos64, int64_t, 8)
GENERIC_ATOX_OPS(aotou8, strtou8, uint8_t, 8)
GENERIC_ATOX_OPS(aotou16, strtou16, uint16_t, 8)
GENERIC_ATOX_OPS(aotou32, strtou32, uint32_t, 8)
GENERIC_ATOX_OPS(aotou64, strtou64, uint64_t, 8)

GENERIC_ATOX_OPS(ahtoi, strtoi, int, 16)
GENERIC_ATOX_OPS(ahtol, strtol, long, 16)
GENERIC_ATOX_OPS(ahtoll, strtoll, long long, 16)
GENERIC_ATOX_OPS(ahtoimax, strtoimax, intmax_t, 16)
GENERIC_ATOX_OPS(ahtoui, strtoui, unsigned int, 16)
GENERIC_ATOX_OPS(ahtoul, strtoul, unsigned long, 16)
GENERIC_ATOX_OPS(ahtoull, strtoull, unsigned long long, 16)
GENERIC_ATOX_OPS(ahtoumax, strtoumax, uintmax_t, 16)
GENERIC_ATOX_OPS(ahtos8, strtos8, int8_t, 16)
GENERIC_ATOX_OPS(ahtos16, strtos16, int16_t, 16)
GENERIC_ATOX_OPS(ahtos32, strtos32, int32_t, 16)
GENERIC_ATOX_OPS(ahtos64, strtos64, int64_t, 16)
GENERIC_ATOX_OPS(ahtou8, strtou8, uint8_t, 16)
GENERIC_ATOX_OPS(ahtou16, strtou16, uint16_t, 16)
GENERIC_ATOX_OPS(ahtou32, strtou32, uint32_t, 16)
GENERIC_ATOX_OPS(ahtou64, strtou64, uint64_t, 16)

#endif  /* _STDLIB_H_ */
