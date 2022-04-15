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

unsigned long strtoul(const char *nptr)
{
    unsigned long total = 0;
    char div, c;

    if (!nptr || !*nptr)
        return 0;

    while (isspace((int)*nptr))
        ++nptr;

    c = *nptr++;
    if (c == '0' && tolower(*nptr) == 'x') {
        div = 16;
        c = *(++nptr);
        nptr++;
    } else {
        div = 10;
    }

    while (c) {
        if (isdigit(c))
            total = div * total + (c - '0');
        else if (div == 16 && c >= 'a' && c <= 'f')
            total = div * total + (c - 'a' + 10);
        else if (div == 16 && c >= 'A' && c <= 'F')
            total = div * total + (c - 'A' + 10);
        else
            return 0;
        c = *nptr++;
    }

    return total;
}
EXPORT_SYMBOL(strtoul);

unsigned long long strtoull(const char *nptr)
{
    unsigned long long total = 0;
    char div, c;

    if (!nptr || !*nptr)
        return 0;

    while (isspace((int)*nptr))
        ++nptr;

    c = *nptr++;
    if (c == '0' && tolower(*nptr) == 'x') {
        div = 16;
        c = *(++nptr);
        nptr++;
    } else {
        div = 10;
    }

    while (c) {
        if (isdigit(c))
            total = div * total + (c - '0');
        else if (div == 16 && c >= 'a' && c <= 'f')
            total = div * total + (c - 'a' + 10);
        else if (div == 16 && c >= 'A' && c <= 'F')
            total = div * total + (c - 'A' + 10);
        else
            return 0;
        c = *nptr++;
    }

    return total;
}
EXPORT_SYMBOL(strtoull);
