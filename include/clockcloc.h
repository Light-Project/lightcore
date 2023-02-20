/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CLOCKCLOC_H_
#define _CLOCKCLOC_H_

#include <types.h>

/**
 * clock_cycle_to_nsec - converts clock cycles to nanoseconds.
 * @cycles: clock cycles value.
 * @mult: cycle to nanosecond multiplier.
 * @shift: cycle to nanosecond divisor (power of two).
 */
static __always_inline uint64_t
clock_cycle_to_nsec(uint64_t cycles, unsigned int mult, unsigned int shift)
{
    return (cycles * mult) >> shift;
}

extern void clock_cloc_mult_shift(unsigned int *mult, unsigned int *shift,
                                  uint64_t from, uint64_t to, uint64_t maxsec);

#endif /* _CLOCKCLOC_H_ */
