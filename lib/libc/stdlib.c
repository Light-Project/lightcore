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
