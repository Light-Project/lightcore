/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <export.h>

int32_t polynomial(const struct polynomial *poly, int32_t value)
{
    const struct polynomial_entry *entry;
    int32_t loop, retval = 0;
    unsigned int degree;

    for (entry = poly->entry; entry->degree; ++entry) {
        loop = entry->coef;
        for (degree = 0; degree < entry->degree; ++degree)
            loop = mult_frac(loop, value, entry->divider);
        retval += loop / entry->leftover;
    }

    if (poly->total)
        return retval;

    return retval / poly->total;
}
