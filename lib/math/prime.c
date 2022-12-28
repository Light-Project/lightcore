/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <limits.h>
#include <export.h>

bool prime_check(unsigned long num)
{
    unsigned long walk;

    for (walk = sqrtl(num); walk > 1; --walk) {
        if ((num % walk) == 0)
            break;
    }

    return walk == 1;
}
EXPORT_SYMBOL(prime_check);

unsigned long prime_next(unsigned long num)
{
    while (num < ULONG_MAX) {
        if (prime_check(++num))
            break;
    }

    return num;
}
EXPORT_SYMBOL(prime_next);
