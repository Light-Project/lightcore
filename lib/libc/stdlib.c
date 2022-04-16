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

int atoi(const char *nptr)
{
    int total = 0;
    char sign, c;

    if (!nptr || !*nptr)
        return 0;

    while (isspace((int)*nptr))
        ++nptr;

    c = sign = *nptr++;
    if (c == '-' || c == '+')
        c = *nptr++;

    while (isdigit(c)) {
        total = 10 * total + (c - '0');
        c = *nptr++;
    }

    if (sign == '-')
        return -total;

    return total;
}
EXPORT_SYMBOL(atoi);

#define GENERIC_STRNTOX_OPS(name, type, signed)                         \
type name(const char *nptr, unsigned int len)                           \
{                                                                       \
    type total = 0;                                                     \
    char sign, c, div = 10;                                             \
                                                                        \
    if (!nptr || !*nptr)                                                \
        return 0;                                                       \
                                                                        \
    while (isspace((int)*nptr) && len--)                                \
        ++nptr;                                                         \
                                                                        \
    if (!len)                                                           \
        return 0;                                                       \
                                                                        \
    c = sign = *nptr++;                                                 \
    if (signed && (c == '-' || c == '+') && len--) {                    \
        c = *nptr++;                                                    \
        if (!len)                                                       \
            return 0;                                                   \
    }                                                                   \
                                                                        \
    if (c == '0' && tolower(*nptr) == 'x' && len > 2) {                 \
        div = 16;                                                       \
        c = *(++nptr);                                                  \
        nptr++;                                                         \
        len -= 2;                                                       \
    }                                                                   \
                                                                        \
    while (c && len--) {                                                \
        if (isdigit(c))                                                 \
            total = div * total + (c - '0');                            \
        else if (div == 16 && tolower(c) >= 'a' && tolower(c) <= 'f')   \
            total = div * total + (tolower(c) - 'a' + 10);              \
        else                                                            \
            return 0;                                                   \
        c = *nptr++;                                                    \
    }                                                                   \
                                                                        \
    if (signed && sign == '-' && div == 10)                             \
        return -total;                                                  \
                                                                        \
    return total;                                                       \
}

GENERIC_STRNTOX_OPS(strntol, long, true)
GENERIC_STRNTOX_OPS(strntoll, long long, true)
GENERIC_STRNTOX_OPS(strntoul, unsigned long, false)
GENERIC_STRNTOX_OPS(strntoull, unsigned long long, false)

EXPORT_SYMBOL(strntol);
EXPORT_SYMBOL(strntoll);
EXPORT_SYMBOL(strntoul);
EXPORT_SYMBOL(strntoull);

#define GENERIC_STRTOX_OPS(name, func, type)                            \
type name(const char *nptr)                                             \
{                                                                       \
    return func(nptr, UINT_MAX);                                        \
}

GENERIC_STRTOX_OPS(strtol, strntol, long)
GENERIC_STRTOX_OPS(strtoll, strntoll, long long)
GENERIC_STRTOX_OPS(strtoul, strntoul, unsigned long)
GENERIC_STRTOX_OPS(strtoull, strntoull, unsigned long long)

EXPORT_SYMBOL(strtol);
EXPORT_SYMBOL(strtoll);
EXPORT_SYMBOL(strtoul);
EXPORT_SYMBOL(strtoull);
