/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <libgcc.h>
#include <initcall.h>
#include <export.h>

unsigned long __weak __udivmodsi4(unsigned long u, unsigned long d, int modwanted)
{
    unsigned long bit = 1;
    unsigned long res = 0;

    while (d < u && bit && !(d & BIT(BITS_PER_LONG - 1))) {
        d <<= 1;
        bit <<= 1;
    }

    while (bit) {
        if (u >= d) {
            u -= d;
            res |= bit;
        }
        bit >>= 1;
        d >>= 1;
    }

    return modwanted ? u : res;
}
EXPORT_SYMBOL(__udivmodsi4);

unsigned long __weak __udivsi3(unsigned long u, unsigned long v)
{
    return __udivmodsi4(u, v, false);
}
EXPORT_SYMBOL(__udivsi3);

unsigned long __weak __umodsi3(unsigned long u, unsigned long v)
{
    return __udivmodsi4(u, v, true);
}
EXPORT_SYMBOL(__umodsi3);

long __divmodsi4(long u, long v, int modwanted)
{
    unsigned int neg1 = 0, neg2 = 0;
    long res;

    if (u < 0) {
        neg1 = ~neg1;
        neg2 = ~neg2;
        u = -u;
    }

    if (v < 0) {
        neg1 = ~neg1;
        v = -v;
    }

    res = __udivmodsi4(u, v, modwanted);
    if (modwanted) {
        if (neg2)
            res = -res;
    } else if (neg1)
        res = -res;

    return res;
}
EXPORT_SYMBOL(__divmodsi4);

long __weak __divsi3(long u, long v)
{
    unsigned int neg = 0;
    long res;

    if (u < 0) {
        neg = ~neg;
        u = -u;
    }

    if (v < 0) {
        neg = ~neg;
        v = -v;
    }

    res = __udivmodsi4(u, v, false);
    return neg ? -res : res;
}
EXPORT_SYMBOL(__divsi3);

long __weak __modsi3(long u, long v)
{
    unsigned int neg = 0;
    long res;

    if (u < 0) {
        neg = ~neg;
        u = -u;
    }

    if (v < 0)
        v = -v;

    res = __udivmodsi4(u, v, true);
    return neg ? -res : res;
}
EXPORT_SYMBOL(__modsi3);
