/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <libgcc.h>
#include <bitops.h>
#include <initcall.h>
#include <export.h>

UDItype __weak __udivmodsi4(UDItype u, UDItype d, int modwanted)
{
    UDItype bit = 1;
    UDItype res = 0;

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

UDItype __weak __udivsi3(UDItype u, UDItype v)
{
    return __udivmodsi4(u, v, false);
}
EXPORT_SYMBOL(__udivsi3);

UDItype __weak __umodsi3(UDItype u, UDItype v)
{
    return __udivmodsi4(u, v, true);
}
EXPORT_SYMBOL(__umodsi3);

DItype __divmodsi4(DItype u, DItype v, int modwanted)
{
    Wtype neg1 = 0, neg2 = 0;
    DItype res;

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

DItype __weak __divsi3(DItype u, DItype v)
{
    Wtype neg = 0;
    DItype res;

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

DItype __weak __modsi3(DItype u, DItype v)
{
    Wtype neg = 0;
    DItype res;

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
