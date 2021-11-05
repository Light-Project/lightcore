/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITS_H_
#define _BITS_H_

#include <lightcore/asm/bitsperlong.h>

#define BITS_PER_BYTE       8
#define BITS_PER_TYPE(type) (sizeof(type) * BITS_PER_BYTE)
#define BITS_TO_U8(nr)      (nr / BITS_PER_TYPE(uint8_t))
#define BITS_TO_U16(nr)     (nr / BITS_PER_TYPE(uint16_t))
#define BITS_TO_U32(nr)     (nr / BITS_PER_TYPE(uint32_t))
#define BITS_TO_U64(nr)     (nr / BITS_PER_TYPE(uint64_t))
#define BITS_TO_LONG(nr)    (nr / BITS_PER_TYPE(long))

#define BIT(nr) (1UL << ((nr) % BITS_PER_LONG))
#define BIT_SHIFT(shift, val) ((val) << ((shift) % BITS_PER_LONG))

#define BIT_RANGE(hi, lo) \
    (((~0UL) - (1UL << (lo)) + 1) & \
    (~0UL >> (BITS_PER_LONG - 1 - (hi))))

#endif  /* _BITS_H_ */
