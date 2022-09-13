/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BCD_H_
#define _BCD_H_

#include <types.h>

#define bcd2bin_const(bcd) (            \
    ((bcd) & 0x0f) + ((bcd) >> 4) * 10  \
)

#define bin2bcd_const(bin) (            \
    (((bin) / 10) << 4) + (bin) % 10    \
)

/**
 * bcd2bin - convert bcd to bin.
 * @bcd: the bcd to convert.
 */
#define bcd2bin(bcd) (                      \
    __builtin_constant_p((uint8_t)(bcd))    \
    ? bcd2bin_const(bcd)                    \
    : bcd2bin_dynamic(bcd)                  \
)

/**
 * bin2bcd - convert bin to bcd.
 * @bcd: the bin to convert.
 */
#define bin2bcd(bin) (                      \
    __builtin_constant_p((uint8_t)(bin))    \
    ? bin2bcd_const(bin)                    \
    : bin2bcd_dynamic(bin)                  \
)

extern __attribute_const__ uint8_t bcd2bin_dynamic(uint8_t bcd);
extern __attribute_const__ uint8_t bin2bcd_dynamic(uint8_t bin);

#endif /* _BCD_H_ */
