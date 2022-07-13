/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <kernel.h>
#include <ctype.h>
#include <export.h>

int chtoi(unsigned char ch)
{
    unsigned char cu = ch & 0xdf;
    return -1 +
        ((ch - '0' +  1) & (unsigned)((ch - '9' - 1) & ('0' - 1 - ch)) >> 8) +
        ((cu - 'A' + 11) & (unsigned)((cu - 'F' - 1) & ('A' - 1 - cu)) >> 8);
}
EXPORT_SYMBOL(chtoi);

#define GENERIC_XBNTOA_OPS(name, type, signed)              \
int name(type val, char *buff, int base, size_t length)     \
{                                                           \
    char brev[64 + 3], conv[] = "0123456789abcdef";         \
    unsigned int used = 0, index = 0;                       \
    bool negative = false;                                  \
                                                            \
    if (signed && val < 0) {                                \
        negative = true;                                    \
        val = -val;                                         \
    }                                                       \
                                                            \
    do {                                                    \
        brev[index++] = conv[val % base];                   \
        val /= base;                                        \
    } while (val);                                          \
                                                            \
    if (base == 8)                                          \
        brev[index++] = '0';                                \
    else if (base == 16) {                                  \
        brev[index++] = 'x';                                \
        brev[index++] = '0';                                \
    } else if (base == 2) {                                 \
        brev[index++] = 'b';                                \
        brev[index++] = '0';                                \
    }                                                       \
                                                            \
    if (signed && negative)                                 \
        brev[index++] = '-';                                \
                                                            \
    while (length > (used + 1) && index) {                  \
        *buff++ = brev[--index];                            \
        used++;                                             \
    }                                                       \
                                                            \
    if (length)                                             \
        *buff = '\0';                                       \
                                                            \
    used += index;                                          \
    return used;                                            \
}

GENERIC_XBNTOA_OPS(intoa, int, true)
GENERIC_XBNTOA_OPS(lntoa, long, true)
GENERIC_XBNTOA_OPS(llntoa, long long, true)
GENERIC_XBNTOA_OPS(imaxntoa, intmax_t, true)
GENERIC_XBNTOA_OPS(uintoa, unsigned int, false)
GENERIC_XBNTOA_OPS(ulntoa, unsigned long, false)
GENERIC_XBNTOA_OPS(ullntoa, unsigned long long, false)
GENERIC_XBNTOA_OPS(umaxntoa, uintmax_t, false)
GENERIC_XBNTOA_OPS(s8ntoa, int8_t, true)
GENERIC_XBNTOA_OPS(s16ntoa, int16_t, true)
GENERIC_XBNTOA_OPS(s32ntoa, int32_t, true)
GENERIC_XBNTOA_OPS(s64ntoa, int64_t, true)
GENERIC_XBNTOA_OPS(u8ntoa, uint8_t, false)
GENERIC_XBNTOA_OPS(u16ntoa, uint16_t, false)
GENERIC_XBNTOA_OPS(u32ntoa, uint32_t, false)
GENERIC_XBNTOA_OPS(u64ntoa, uint64_t, false)

EXPORT_SYMBOL(intoa);
EXPORT_SYMBOL(lntoa);
EXPORT_SYMBOL(llntoa);
EXPORT_SYMBOL(imaxntoa);
EXPORT_SYMBOL(uintoa);
EXPORT_SYMBOL(ulntoa);
EXPORT_SYMBOL(ullntoa);
EXPORT_SYMBOL(umaxntoa);
EXPORT_SYMBOL(s8ntoa);
EXPORT_SYMBOL(s16ntoa);
EXPORT_SYMBOL(s32ntoa);
EXPORT_SYMBOL(s64ntoa);
EXPORT_SYMBOL(u8ntoa);
EXPORT_SYMBOL(u16ntoa);
EXPORT_SYMBOL(u32ntoa);
EXPORT_SYMBOL(u64ntoa);

#define GENERIC_XBTOA_OPS(name, func, type)     \
int name(type val, char *str, int base)         \
{                                               \
    return func(val, str, base, ULONG_MAX);     \
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

EXPORT_SYMBOL(itoa);
EXPORT_SYMBOL(ltoa);
EXPORT_SYMBOL(lltoa);
EXPORT_SYMBOL(imaxtoa);
EXPORT_SYMBOL(uitoa);
EXPORT_SYMBOL(ultoa);
EXPORT_SYMBOL(ulltoa);
EXPORT_SYMBOL(umaxtoa);
EXPORT_SYMBOL(s8toa);
EXPORT_SYMBOL(s16toa);
EXPORT_SYMBOL(s32toa);
EXPORT_SYMBOL(s64toa);
EXPORT_SYMBOL(u8toa);
EXPORT_SYMBOL(u16toa);
EXPORT_SYMBOL(u32toa);
EXPORT_SYMBOL(u64toa);

#define GENERIC_XTOA_OPS(name, func, type, base)    \
int name(type val, char *str)                       \
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

EXPORT_SYMBOL(itoad);
EXPORT_SYMBOL(ltoad);
EXPORT_SYMBOL(lltoad);
EXPORT_SYMBOL(imaxtoad);
EXPORT_SYMBOL(uitoad);
EXPORT_SYMBOL(ultoad);
EXPORT_SYMBOL(ulltoad);
EXPORT_SYMBOL(umaxtoad);
EXPORT_SYMBOL(s8toad);
EXPORT_SYMBOL(s16toad);
EXPORT_SYMBOL(s32toad);
EXPORT_SYMBOL(s64toad);
EXPORT_SYMBOL(u8toad);
EXPORT_SYMBOL(u16toad);
EXPORT_SYMBOL(u32toad);
EXPORT_SYMBOL(u64toad);

EXPORT_SYMBOL(itoab);
EXPORT_SYMBOL(ltoab);
EXPORT_SYMBOL(lltoab);
EXPORT_SYMBOL(imaxtoab);
EXPORT_SYMBOL(uitoab);
EXPORT_SYMBOL(ultoab);
EXPORT_SYMBOL(ulltoab);
EXPORT_SYMBOL(umaxtoab);
EXPORT_SYMBOL(s8toab);
EXPORT_SYMBOL(s16toab);
EXPORT_SYMBOL(s32toab);
EXPORT_SYMBOL(s64toab);
EXPORT_SYMBOL(u8toab);
EXPORT_SYMBOL(u16toab);
EXPORT_SYMBOL(u32toab);
EXPORT_SYMBOL(u64toab);

EXPORT_SYMBOL(itoao);
EXPORT_SYMBOL(ltoao);
EXPORT_SYMBOL(lltoao);
EXPORT_SYMBOL(imaxtoao);
EXPORT_SYMBOL(uitoao);
EXPORT_SYMBOL(ultoao);
EXPORT_SYMBOL(ulltoao);
EXPORT_SYMBOL(umaxtoao);
EXPORT_SYMBOL(s8toao);
EXPORT_SYMBOL(s16toao);
EXPORT_SYMBOL(s32toao);
EXPORT_SYMBOL(s64toao);
EXPORT_SYMBOL(u8toao);
EXPORT_SYMBOL(u16toao);
EXPORT_SYMBOL(u32toao);
EXPORT_SYMBOL(u64toao);

EXPORT_SYMBOL(itoah);
EXPORT_SYMBOL(ltoah);
EXPORT_SYMBOL(lltoah);
EXPORT_SYMBOL(imaxtoah);
EXPORT_SYMBOL(uitoah);
EXPORT_SYMBOL(ultoah);
EXPORT_SYMBOL(ulltoah);
EXPORT_SYMBOL(umaxtoah);
EXPORT_SYMBOL(s8toah);
EXPORT_SYMBOL(s16toah);
EXPORT_SYMBOL(s32toah);
EXPORT_SYMBOL(s64toah);
EXPORT_SYMBOL(u8toah);
EXPORT_SYMBOL(u16toah);
EXPORT_SYMBOL(u32toah);
EXPORT_SYMBOL(u64toah);

#define GENERIC_STRNTOX_OPS(name, type, signed)                                 \
type name(const char *nptr, char **endptr, unsigned int base, size_t length)    \
{                                                                               \
    type total, value;                                                          \
    char sign;                                                                  \
                                                                                \
    if (!nptr || !*nptr)                                                        \
        return 0;                                                               \
                                                                                \
    while (isspace(*nptr) && length--)                                          \
        ++nptr;                                                                 \
                                                                                \
    if (!length)                                                                \
        return 0;                                                               \
                                                                                \
    sign = *nptr;                                                               \
    if (signed && (sign == '-' || sign == '+')) {                               \
        nptr++;                                                                 \
        if (!--length)                                                          \
            return 0;                                                           \
    }                                                                           \
                                                                                \
    if (!base) {                                                                \
        if (nptr[0] == '0') {                                                   \
            if (length > 2 && tolower(nptr[1]) == 'x') {                        \
                base  = 16;                                                     \
                nptr += 2;                                                      \
                length  -= 2;                                                   \
            } else if (length > 2 && tolower(nptr[1]) == 'b') {                 \
                base  = 2;                                                      \
                nptr += 2;                                                      \
                length  -= 2;                                                   \
            } else if (length > 1) {                                            \
                base  = 8;                                                      \
                nptr += 1;                                                      \
                length  -= 1;                                                   \
            }                                                                   \
        } else                                                                  \
            base = 10;                                                          \
    } else if (nptr[0] == '0') {                                                \
        if (base == 16 && length > 2 && tolower(nptr[1]) == 'x') {              \
            nptr += 2;                                                          \
            length  -= 2;                                                       \
        } else if (base == 2 && length > 2 && tolower(nptr[1]) == 'b') {        \
            nptr += 2;                                                          \
            length  -= 2;                                                       \
        } else if (base == 8 && length > 1) {                                   \
            nptr += 1;                                                          \
            length  -= 1;                                                       \
        }                                                                       \
    }                                                                           \
                                                                                \
    for (total = 0; *nptr && length--; nptr++) {                                \
        if (isdigit(*nptr))                                                     \
            value = *nptr - '0';                                                \
        else if (tolower(*nptr) >= 'a' && tolower(*nptr) <= 'f')                \
            value = tolower(*nptr) - 'a' + 10;                                  \
        else                                                                    \
            break;                                                              \
                                                                                \
        if (value >= base)                                                      \
            break;                                                              \
                                                                                \
        total *= base;                                                          \
        total += value;                                                         \
    }                                                                           \
                                                                                \
    if (endptr)                                                                 \
        *endptr = (char *)nptr;                                                 \
                                                                                \
    return signed && sign == '-' ? -total : total;                              \
}                                                                               \

GENERIC_STRNTOX_OPS(strntoi, int, true)
GENERIC_STRNTOX_OPS(strntol, long, true)
GENERIC_STRNTOX_OPS(strntoll, long long, true)
GENERIC_STRNTOX_OPS(strntoimax, intmax_t, true)
GENERIC_STRNTOX_OPS(strntoui, unsigned int, false)
GENERIC_STRNTOX_OPS(strntoul, unsigned long, false)
GENERIC_STRNTOX_OPS(strntoull, unsigned long long, false)
GENERIC_STRNTOX_OPS(strntoumax, uintmax_t, false)
GENERIC_STRNTOX_OPS(strntos8, int8_t, true)
GENERIC_STRNTOX_OPS(strntos16, int16_t, true)
GENERIC_STRNTOX_OPS(strntos32, int32_t, true)
GENERIC_STRNTOX_OPS(strntos64, int64_t, true)
GENERIC_STRNTOX_OPS(strntou8, uint8_t, false)
GENERIC_STRNTOX_OPS(strntou16, uint16_t, false)
GENERIC_STRNTOX_OPS(strntou32, uint32_t, false)
GENERIC_STRNTOX_OPS(strntou64, uint64_t, false)

EXPORT_SYMBOL(strntoi);
EXPORT_SYMBOL(strntol);
EXPORT_SYMBOL(strntoll);
EXPORT_SYMBOL(strntoimax);
EXPORT_SYMBOL(strntoui);
EXPORT_SYMBOL(strntoul);
EXPORT_SYMBOL(strntoull);
EXPORT_SYMBOL(strntoumax);
EXPORT_SYMBOL(strntos8);
EXPORT_SYMBOL(strntos16);
EXPORT_SYMBOL(strntos32);
EXPORT_SYMBOL(strntos64);
EXPORT_SYMBOL(strntou8);
EXPORT_SYMBOL(strntou16);
EXPORT_SYMBOL(strntou32);
EXPORT_SYMBOL(strntou64);

#define GENERIC_STRTOX_OPS(name, func, type)                    \
type name(const char *nptr, char **endptr, unsigned int base)   \
{                                                               \
    return func(nptr, endptr, base, ULONG_MAX);                 \
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

EXPORT_SYMBOL(strtoi);
EXPORT_SYMBOL(strtol);
EXPORT_SYMBOL(strtoll);
EXPORT_SYMBOL(strtoimax);
EXPORT_SYMBOL(strtoui);
EXPORT_SYMBOL(strtoul);
EXPORT_SYMBOL(strtoull);
EXPORT_SYMBOL(strtoumax);
EXPORT_SYMBOL(strtos8);
EXPORT_SYMBOL(strtos16);
EXPORT_SYMBOL(strtos32);
EXPORT_SYMBOL(strtos64);
EXPORT_SYMBOL(strtou8);
EXPORT_SYMBOL(strtou16);
EXPORT_SYMBOL(strtou32);
EXPORT_SYMBOL(strtou64);

#define GENERIC_AXTOX_OPS(name, func, type) \
type name(const char *nptr)                 \
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

EXPORT_SYMBOL(axtoi);
EXPORT_SYMBOL(axtol);
EXPORT_SYMBOL(axtoll);
EXPORT_SYMBOL(axtoimax);
EXPORT_SYMBOL(axtoui);
EXPORT_SYMBOL(axtoul);
EXPORT_SYMBOL(axtoull);
EXPORT_SYMBOL(axtoumax);
EXPORT_SYMBOL(axtos8);
EXPORT_SYMBOL(axtos16);
EXPORT_SYMBOL(axtos32);
EXPORT_SYMBOL(axtos64);
EXPORT_SYMBOL(axtou8);
EXPORT_SYMBOL(axtou16);
EXPORT_SYMBOL(axtou32);
EXPORT_SYMBOL(axtou64);

#define GENERIC_ATOX_OPS(name, func, type, base)    \
type name(const char *nptr)                         \
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

EXPORT_SYMBOL(atoi);
EXPORT_SYMBOL(atol);
EXPORT_SYMBOL(atoll);
EXPORT_SYMBOL(atoimax);
EXPORT_SYMBOL(atoui);
EXPORT_SYMBOL(atoul);
EXPORT_SYMBOL(atoull);
EXPORT_SYMBOL(atoumax);
EXPORT_SYMBOL(atos8);
EXPORT_SYMBOL(atos16);
EXPORT_SYMBOL(atos32);
EXPORT_SYMBOL(atos64);
EXPORT_SYMBOL(atou8);
EXPORT_SYMBOL(atou16);
EXPORT_SYMBOL(atou32);
EXPORT_SYMBOL(atou64);

EXPORT_SYMBOL(abtoi);
EXPORT_SYMBOL(abtol);
EXPORT_SYMBOL(abtoll);
EXPORT_SYMBOL(abtoimax);
EXPORT_SYMBOL(abtoui);
EXPORT_SYMBOL(abtoul);
EXPORT_SYMBOL(abtoull);
EXPORT_SYMBOL(abtoumax);
EXPORT_SYMBOL(abtos8);
EXPORT_SYMBOL(abtos16);
EXPORT_SYMBOL(abtos32);
EXPORT_SYMBOL(abtos64);
EXPORT_SYMBOL(abtou8);
EXPORT_SYMBOL(abtou16);
EXPORT_SYMBOL(abtou32);
EXPORT_SYMBOL(abtou64);

EXPORT_SYMBOL(aotoi);
EXPORT_SYMBOL(aotol);
EXPORT_SYMBOL(aotoll);
EXPORT_SYMBOL(aotoimax);
EXPORT_SYMBOL(aotoui);
EXPORT_SYMBOL(aotoul);
EXPORT_SYMBOL(aotoull);
EXPORT_SYMBOL(aotoumax);
EXPORT_SYMBOL(aotos8);
EXPORT_SYMBOL(aotos16);
EXPORT_SYMBOL(aotos32);
EXPORT_SYMBOL(aotos64);
EXPORT_SYMBOL(aotou8);
EXPORT_SYMBOL(aotou16);
EXPORT_SYMBOL(aotou32);
EXPORT_SYMBOL(aotou64);

EXPORT_SYMBOL(ahtoi);
EXPORT_SYMBOL(ahtol);
EXPORT_SYMBOL(ahtoll);
EXPORT_SYMBOL(ahtoimax);
EXPORT_SYMBOL(ahtoui);
EXPORT_SYMBOL(ahtoul);
EXPORT_SYMBOL(ahtoull);
EXPORT_SYMBOL(ahtoumax);
EXPORT_SYMBOL(ahtos8);
EXPORT_SYMBOL(ahtos16);
EXPORT_SYMBOL(ahtos32);
EXPORT_SYMBOL(ahtos64);
EXPORT_SYMBOL(ahtou8);
EXPORT_SYMBOL(ahtou16);
EXPORT_SYMBOL(ahtou32);
EXPORT_SYMBOL(ahtou64);
