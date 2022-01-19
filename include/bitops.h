/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITOPS_H_
#define _BITOPS_H_

#include <kernel.h>
#include <bits.h>
#include <asm/bitops.h>

#define BITS_TO_U8(nr)          DIV_ROUND_UP(nr, BITS_PER_U8)
#define BITS_TO_U16(nr)         DIV_ROUND_UP(nr, BITS_PER_U16)
#define BITS_TO_U32(nr)         DIV_ROUND_UP(nr, BITS_PER_U32)
#define BITS_TO_U64(nr)         DIV_ROUND_UP(nr, BITS_PER_U64)
#define BITS_TO_LONG(nr)        DIV_ROUND_UP(nr, BITS_PER_LONG)
#define BITS_TO_LONG_LONG(nr)   DIV_ROUND_UP(nr, BITS_PER_LONG_LONG)

#endif  /* _BITOPS_H_ */
