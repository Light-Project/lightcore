/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BCD_H_
#define _BCD_H_

#include <types.h>
#include <limits.h>

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

extern const uint8_t bcd2bin_table[256];
extern const uint8_t bin2bcd_table[256];

static __attribute_const__ inline
uint8_t bcd2bin_dynamic(uint8_t bcd)
{
    return bcd2bin_table[bcd];
}

static inline __attribute_const__
uint8_t bin2bcd_dynamic(uint8_t bin)
{
    return bin2bcd_table[bin];
}

#endif /* _BCD_H_ */
