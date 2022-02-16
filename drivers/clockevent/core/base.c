/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "clkevt"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/clockevent.h>
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

/**
 * clock_cycle_to_nsec - converts clock cycles to nanoseconds
 * @cycles: cycles
 * @mult: cycle to nanosecond multiplier
 * @shift: cycle to nanosecond divisor (power of two)
 */
uint64_t clock_cycle_to_nsec(uint64_t cycles, unsigned int mult, unsigned int shift)
{
    return (cycles * mult) >> shift;
}
EXPORT_SYMBOL(clock_cycle_to_nsec);
