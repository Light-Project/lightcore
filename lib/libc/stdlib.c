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

unsigned char itoch(int val)
{
    const char conv[] = "0123456789abcdef";

    if (unlikely(val > sizeof(conv)))
        val %= sizeof(conv);

    return (unsigned char)conv[val];
}
EXPORT_SYMBOL(itoch);

#define GENERIC_XBNTOA_OPS(name, type, signed)              \
int name(type val, char *buff, int base, size_t length)     \
{                                                           \
    unsigned int used = 0, index = 0;                       \
    bool negative = false;                                  \
    char brev[64 + 3];                                      \
                                                            \
    if (signed && val < 0) {                                \
        negative = true;                                    \
        val = -val;                                         \
    }                                                       \
                                                            \
    do {                                                    \
        brev[index++] = itoch(val % base);                  \
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

GENERIC_XBNTOA_OPS(s8ntoa, int8_t, true)
GENERIC_XBNTOA_OPS(s16ntoa, int16_t, true)
GENERIC_XBNTOA_OPS(s32ntoa, int32_t, true)
GENERIC_XBNTOA_OPS(s64ntoa, int64_t, true)
GENERIC_XBNTOA_OPS(u8ntoa, uint8_t, false)
GENERIC_XBNTOA_OPS(u16ntoa, uint16_t, false)
GENERIC_XBNTOA_OPS(u32ntoa, uint32_t, false)
GENERIC_XBNTOA_OPS(u64ntoa, uint64_t, false)

#define INTOA_ALIAS __stringify(__PASTE(__PASTE(s, INT_WIDTH), ntoa))
#define LNTOA_ALIAS __stringify(__PASTE(__PASTE(s, LONG_WIDTH), ntoa))
#define LLNTOA_ALIAS __stringify(__PASTE(__PASTE(s, LLONG_WIDTH), ntoa))
#define IMAXNTOA_ALIAS __stringify(__PASTE(__PASTE(s, INTMAX_WIDTH), ntoa))
#define UINTOA_ALIAS __stringify(__PASTE(__PASTE(u, INT_WIDTH), ntoa))
#define ULNTOA_ALIAS __stringify(__PASTE(__PASTE(u, LONG_WIDTH), ntoa))
#define ULLNTOA_ALIAS __stringify(__PASTE(__PASTE(u, LLONG_WIDTH), ntoa))
#define UMAXNTOA_ALIAS __stringify(__PASTE(__PASTE(u, INTMAX_WIDTH), ntoa))

extern int intoa(int val, char *buff, int base, size_t length) __alias(INTOA_ALIAS);
extern int lntoa(long val, char *buff, int base, size_t length) __alias(LNTOA_ALIAS);
extern int llntoa(long long val, char *buff, int base, size_t length) __alias(LLNTOA_ALIAS);
extern int imaxntoa(intmax_t val, char *buff, int base, size_t length) __alias(IMAXNTOA_ALIAS);
extern int uintoa(unsigned int val, char *buff, int base, size_t length) __alias(UINTOA_ALIAS);
extern int ulntoa(unsigned long val, char *buff, int base, size_t length) __alias(ULNTOA_ALIAS);
extern int ullntoa(unsigned long long val, char *buff, int base, size_t length) __alias(ULLNTOA_ALIAS);
extern int umaxntoa(uintmax_t val, char *buff, int base, size_t length) __alias(UMAXNTOA_ALIAS);

EXPORT_SYMBOL(s8ntoa);
EXPORT_SYMBOL(s16ntoa);
EXPORT_SYMBOL(s32ntoa);
EXPORT_SYMBOL(s64ntoa);
EXPORT_SYMBOL(u8ntoa);
EXPORT_SYMBOL(u16ntoa);
EXPORT_SYMBOL(u32ntoa);
EXPORT_SYMBOL(u64ntoa);

EXPORT_SYMBOL(intoa);
EXPORT_SYMBOL(lntoa);
EXPORT_SYMBOL(llntoa);
EXPORT_SYMBOL(imaxntoa);
EXPORT_SYMBOL(uintoa);
EXPORT_SYMBOL(ulntoa);
EXPORT_SYMBOL(ullntoa);
EXPORT_SYMBOL(umaxntoa);

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

GENERIC_STRNTOX_OPS(strntos8, int8_t, true)
GENERIC_STRNTOX_OPS(strntos16, int16_t, true)
GENERIC_STRNTOX_OPS(strntos32, int32_t, true)
GENERIC_STRNTOX_OPS(strntos64, int64_t, true)
GENERIC_STRNTOX_OPS(strntou8, uint8_t, false)
GENERIC_STRNTOX_OPS(strntou16, uint16_t, false)
GENERIC_STRNTOX_OPS(strntou32, uint32_t, false)
GENERIC_STRNTOX_OPS(strntou64, uint64_t, false)

#define STRNTOI_ALIAS __stringify(__PASTE(strntos, INT_WIDTH))
#define STRNTOL_ALIAS __stringify(__PASTE(strntos, LONG_WIDTH))
#define STRNTOLL_ALIAS __stringify(__PASTE(strntos, LLONG_WIDTH))
#define STRNTOIMAX_ALIAS __stringify(__PASTE(strntos, INTMAX_WIDTH))
#define STRNTOUI_ALIAS __stringify(__PASTE(strntou, INT_WIDTH))
#define STRNTOUL_ALIAS __stringify(__PASTE(strntou, LONG_WIDTH))
#define STRNTOULL_ALIAS __stringify(__PASTE(strntou, LLONG_WIDTH))
#define STRNTOUMAX_ALIAS __stringify(__PASTE(strntou, INTMAX_WIDTH))

extern int strntoi(const char *nptr, char **endptr, unsigned int base, size_t length) __alias(STRNTOI_ALIAS);
extern long strntol(const char *nptr, char **endptr, unsigned int base, size_t length) __alias(STRNTOL_ALIAS);
extern long long strntoll(const char *nptr, char **endptr, unsigned int base, size_t length) __alias(STRNTOLL_ALIAS);
extern intmax_t strntoimax(const char *nptr, char **endptr, unsigned int base, size_t length) __alias(STRNTOIMAX_ALIAS);
extern unsigned int strntoui(const char *nptr, char **endptr, unsigned int base, size_t length) __alias(STRNTOUI_ALIAS);
extern unsigned long strntoul(const char *nptr, char **endptr, unsigned int base, size_t length) __alias(STRNTOUL_ALIAS);
extern unsigned long long strntoull(const char *nptr, char **endptr, unsigned int base, size_t length) __alias(STRNTOULL_ALIAS);
extern uintmax_t strntoumax(const char *nptr, char **endptr, unsigned int base, size_t length) __alias(STRNTOUMAX_ALIAS);

EXPORT_SYMBOL(strntos8);
EXPORT_SYMBOL(strntos16);
EXPORT_SYMBOL(strntos32);
EXPORT_SYMBOL(strntos64);
EXPORT_SYMBOL(strntou8);
EXPORT_SYMBOL(strntou16);
EXPORT_SYMBOL(strntou32);
EXPORT_SYMBOL(strntou64);

EXPORT_SYMBOL(strntoi);
EXPORT_SYMBOL(strntol);
EXPORT_SYMBOL(strntoll);
EXPORT_SYMBOL(strntoimax);
EXPORT_SYMBOL(strntoui);
EXPORT_SYMBOL(strntoul);
EXPORT_SYMBOL(strntoull);
EXPORT_SYMBOL(strntoumax);
