/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BITOPS_H_
#define _ASM_GENERIC_BITOPS_H_

static inline unsigned long ffs(unsigned long value)
{
    int shift = 0;

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
    if ((value & 0x1) == 0)
        shift += 1;

    return shift;
}

static inline unsigned long fls(unsigned long value)
{
    int shift = BITS_PER_LONG - 1;

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

#endif  /* _ASM_GENERIC_BITOPS_H_ */
