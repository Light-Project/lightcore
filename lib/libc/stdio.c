/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <kernel.h>
#include <ctype.h>
#include <export.h>

int atoi(const char *nptr)
{
    int total = 0;
    char sign, c;

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

int vsprintf(char *buf, const char *fmt, va_list args)
{
    return vsnprintf(buf, INT_MAX, fmt, args);
}
EXPORT_SYMBOL(vsprintf);
