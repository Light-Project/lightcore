/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BCD_H_
#define _BCD_H_

/**
 * bcd2bin - convert bcd to bin
 * @bcd: the bcd to convert
 */
#define bcd2bin(bcd) (                  \
    ((bcd) & 0x0f) + ((bcd) >> 4) * 10  \
)

/**
 * bin2bcd - convert bin to bcd
 * @bcd: the bin to convert
 */
#define bin2bcd(bin) (                  \
    (((bin) / 10) << 4) + (bin) % 10    \
)

#endif /* _BCD_H_ */
