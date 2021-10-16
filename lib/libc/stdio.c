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

int vsprintf(char *buf, const char *fmt, va_list args)
{
    return vsnprintf(buf, INT_MAX, fmt, args);
}

int snprintf(char *buf, size_t n, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, n, fmt, args);
    va_end(args);

    return i;
}

int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);

    return i;
}

EXPORT_SYMBOL(atoi);
EXPORT_SYMBOL(vsprintf);
EXPORT_SYMBOL(snprintf);
EXPORT_SYMBOL(sprintf);
