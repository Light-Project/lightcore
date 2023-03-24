/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_HWMON_PCF8591_H_
#define _DRIVER_HWMON_PCF8591_H_

#include <bits.h>

#define PCF8591_OUTPUT_EN       BIT(6)              /* Analog output enable */
#define PCF8591_MODE(mode)      BIT_SHIFT(4, mode)  /* Analog input programming */
#define PCF8591_AUTO_INC        BIT(2)              /* Channel auto-increment */
#define PCF8591_CHANNEL(index)  BIT_SHIFT(0, index) /* A/D channel number */

#define PCF8591_MODE_MASK       BIT_RANGE(5, 4)
#define PCF8591_MODE_TWDIFF     0x03 /* Two differential inputs */
#define PCF8591_MODE_SIDIFF     0x02 /* Single-ended differential mixed */
#define PCF8591_MODE_TRDIFF     0x01 /* Three differential inputs */
#define PCF8591_MODE_SINGLE     0x00 /* Four single-ended inputs */

#define PCF8591_CHANNEL_MASK    BIT_RANGE(1, 0)
#define PCF8591_CHANNEL_MAX     3

#endif /* _DRIVER_HWMON_PCF8591_H_ */
