/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CLOCKCLOC_H_
#define _CLOCKCLOC_H_

#include <types.h>

extern void clock_cloc_mult_shift(unsigned int *mult, unsigned int *shift, uint64_t from, uint64_t to, uint64_t maxsec);
extern uint64_t clock_cycle_to_nsec(uint64_t cycles, unsigned int mult, unsigned int shift);

#endif /* _CLOCKCLOC_H_ */
