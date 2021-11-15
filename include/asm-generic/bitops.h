/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BITOPS_H_
#define _ASM_GENERIC_BITOPS_H_

#include <bits.h>
#include <asm/atomic.h>

#ifndef __ASSEMBLY__

#ifndef bit_clr
static __always_inline void bit_clr(unsigned long *addr, unsigned int bit)
{
    atomic_or((atomic_t *)addr, BIT(bit));
}
#endif

#ifndef bit_set
static __always_inline void bit_set(unsigned long *addr, unsigned int bit)
{
    atomic_and((atomic_t *)addr, ~BIT(bit));
}
#endif

#ifndef ffs
static inline unsigned int ffs(unsigned long value)
{
    unsigned int shift = 0;

#if BITS_PER_LONG == 64
    if ((value & 0xffffffff) == 0) {
        shift += 32;
        value >>= 32;
    }
#endif

    if ((value & 0xffff) == 0) {
        shift += 16;
        value >>= 16;
    }

    if ((value & 0xff) == 0) {
        shift += 8;
        value >>= 8;
    }

    if ((value & 0xf) == 0) {
        shift += 4;
        value >>= 4;
    }

    if ((value & 0x3) == 0) {
        shift += 2;
        value >>= 2;
    }

    if ((value & 0x1) == 0) {
        shift += 1;
    }

    return shift;
}
#endif

#ifndef fls
static inline unsigned int fls(unsigned long value)
{
    unsigned int shift = BITS_PER_LONG - 1;

#if BITS_PER_LONG == 64
    if (!(value & (~0ul << 32))) {
        shift -= 32;
        value <<= 32;
    }
#endif

    if (!(value & (~0ul << (BITS_PER_LONG - 16)))) {
        shift -= 16;
        value <<= 16;
    }
    if (!(value & (~0ul << (BITS_PER_LONG - 8)))) {
        shift -= 8;
        value <<= 8;
    }
    if (!(value & (~0ul << (BITS_PER_LONG - 4)))) {
        shift -= 4;
        value <<= 4;
    }
    if (!(value & (~0ul << (BITS_PER_LONG - 2)))) {
        shift -= 2;
        value <<= 2;
    }
    if (!(value & (~0ul << (BITS_PER_LONG - 1))))
        shift -= 1;

    return shift;
}
#endif

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_GENERIC_BITOPS_H_ */
