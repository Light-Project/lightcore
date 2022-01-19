/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <kernel.h>
#include <ctype.h>
#include <export.h>

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
