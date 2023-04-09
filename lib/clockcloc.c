/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "clkcloc"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <clockcloc.h>
#include <export.h>

void clock_cloc_mult_shift(unsigned int *mult, unsigned int *shift,
                           uint64_t from, uint64_t to, uint64_t maxsec)
{
    unsigned int sft, sftacc = 32;
    uint64_t tmp;

    /*
     * Calculate the shift factor which is limiting the conversion
     * range:
     */
    tmp = ((uint64_t)maxsec * from) >> 32;
    while (tmp) {
        tmp >>=1;
        sftacc--;
    }

    /*
     * Find the conversion shift/mult pair which has the best
     * accuracy and fits the maxsec conversion range:
     */
    for (sft = 32; sft > 0; sft--) {
        tmp = (uint64_t) to << sft;
        tmp += from / 2;
        tmp /= from;
        if ((tmp >> sftacc) == 0)
            break;
    }

    *mult = tmp;
    *shift = sft;
}
EXPORT_SYMBOL(clock_cloc_mult_shift);
