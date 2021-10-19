/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITS_H_
#define _BITS_H_

#include <asm-generic/bitsperlong.h>

#define BIT(nr) (1UL << ((nr) % BITS_PER_LONG))
#define BIT_SHIFT(val, shift) ((val) << ((shift) % BITS_PER_LONG))

#define BIT_RANGE(hi, lo) \
    (((~0UL) - (1UL << (lo)) + 1) & \
	(~0UL >> (BITS_PER_LONG - 1 - (hi))))

#endif  /* _BITS_H_ */
