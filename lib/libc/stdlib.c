/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <kernel.h>
#include <ctype.h>
#include <export.h>

char *itoa(int val, char *str, int base)
{
    char conv[] = "0123456789abcdef";
    unsigned char count = 0, tmp[32];
    bool sub = false, hex = false;

    if (val < 0) {
        sub = true;
        val = -val;
    }

    if (base == 16)
        hex = true;

    if (!val) {
        tmp[0] = '0';
        count++;
    }

    while (val != 0) {
        tmp[count] = conv[val % base];
        val /= base;
        count++;
    }

    if (sub && !hex)
        *str++ = '-';

    if (hex) {
        *str++ = '0';
        *str++ = 'x';
    }

    while (count--)
        *str++ = tmp[count];

    *str++ = '\0';

    return str;
}
EXPORT_SYMBOL(itoa);

int chtoi(unsigned char ch)
{
	unsigned char cu = ch & 0xdf;
	return -1 +
		((ch - '0' +  1) & (unsigned)((ch - '9' - 1) & ('0' - 1 - ch)) >> 8) +
		((cu - 'A' + 11) & (unsigned)((cu - 'F' - 1) & ('A' - 1 - cu)) >> 8);
}
EXPORT_SYMBOL(chtoi);

#define GENERIC_STRNTOX_OPS(name, type, signed)                                 \
type name(const char *nptr, char **endptr, unsigned int base, size_t len)       \
{                                                                               \
    type total, value;                                                          \
    char sign;                                                                  \
                                                                                \
    if (!nptr || !*nptr)                                                        \
        return 0;                                                               \
                                                                                \
    while (isspace(*nptr) && len--)                                             \
        ++nptr;                                                                 \
                                                                                \
    if (!len)                                                                   \
        return 0;                                                               \
                                                                                \
    sign = *nptr;                                                               \
    if (signed && (sign == '-' || sign == '+')) {                               \
        nptr++;                                                                 \
        if (!--len)                                                             \
            return 0;                                                           \
    }                                                                           \
                                                                                \
    if (!base) {                                                                \
        if (nptr[0] == '0') {                                                   \
            if (len > 2 && tolower(nptr[1]) == 'x') {                           \
                base  = 16;                                                     \
                nptr += 2;                                                      \
                len  -= 2;                                                      \
            } else if (len > 2 && tolower(nptr[1]) == 'b') {                    \
                base  = 2;                                                      \
                nptr += 2;                                                      \
                len  -= 2;                                                      \
            } else if (len > 1) {                                               \
                base  = 8;                                                      \
                nptr += 1;                                                      \
                len  -= 1;                                                      \
            }                                                                   \
        } else                                                                  \
            base = 10;                                                          \
    } else if (nptr[0] == '0') {                                                \
        if (base == 16 && len > 2 && tolower(nptr[1]) == 'x') {                 \
            nptr += 2;                                                          \
            len  -= 2;                                                          \
        } else if (base == 2 && len > 2 && tolower(nptr[1]) == 'b') {           \
            nptr += 2;                                                          \
            len  -= 2;                                                          \
        } else if (base == 8 && len > 1) {                                      \
            nptr += 1;                                                          \
            len  -= 1;                                                          \
        }                                                                       \
    }                                                                           \
                                                                                \
    for (total = 0; *nptr && len--; nptr++) {                                   \
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
