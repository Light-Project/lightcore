/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <libgcc.h>
#include <initcall.h>
#include <export.h>

unsigned short __udivmodhi4(unsigned short u, unsigned short d, int modwanted)
{
    unsigned short bit = 1;
    unsigned short res = 0;

    while (d < u && bit && !(d & BIT(BITS_PER_U16 - 1))) {
        d <<=1;
        bit <<=1;
    }

    while (bit) {
        if (u >= d) {
            u -= d;
            res |= bit;
        }
        bit >>=1;
        d >>=1;
    }

    return modwanted ? u : res;
}

unsigned short __weak __udivhi3(unsigned short u, unsigned short v)
{
    return __udivmodhi4(u, v, false);
}
EXPORT_SYMBOL(__udivhi3);

unsigned short __weak __umodhi3(unsigned short u, unsigned short v)
{
    return __udivmodhi4(u, v, true);
}
EXPORT_SYMBOL(__umodhi3);

short __weak __divmodhi4(short u, short v, int modwanted)
{
    unsigned int neg1 = 0, neg2 = 0;
    short res;

    if (u < 0) {
        neg1 = ~neg1;
        neg2 = ~neg2;
        u = -u;
    }

    if (v < 0) {
        neg1 = ~neg1;
        v = -v;
    }

    res = __udivmodhi4(u, v, modwanted);
    if (modwanted) {
        if (neg2)
            res = -res;
    } else if (neg1)
        res = -res;

    return res;
}
EXPORT_SYMBOL(__divmodhi4);

short __weak __divhi3(short u, short v)
{
    unsigned int neg = 0;
    short res;

    if (u < 0) {
        neg = ~neg;
        u = -u;
    }

    if (v < 0) {
        neg = ~neg;
        v = -v;
    }

    res = __udivmodhi4(u, v, false);
    return neg ? -res : res;
}
EXPORT_SYMBOL(__divhi3);

short __weak __modhi3(short u, short v)
{
    unsigned int neg = 0;
    short res;

    if (u < 0) {
        neg = ~neg;
        u = -u;
    }

    if (v < 0)
        v = -v;

    res = __udivmodhi4(u, v, true);
    return neg ? -res : res;
}
EXPORT_SYMBOL(__modhi3);
