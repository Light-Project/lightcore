/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_RTC_DS1302_H_
#define _DRIVER_RTC_DS1302_H_

/*
 * Real time clock counts seconds, minutes
 * hours, date of the month, month, day of the
 * week, and year with leap year compensation
 * valid up to 2100
 */

/*
 * command byte:
 *      |1|ram/clk|a4|a3|a2|a1|a0|rw
 * bit:  7    6    5  4  3  2  1  0
 */
enum ds1302_cmd {
    DS1302_SEC          = 0x00 << 1,
    DS1302_MIN          = 0x01 << 1,
    DS1302_HR           = 0x02 << 1,
    DS1302_DATE         = 0x03 << 1,
    DS1302_MONTH        = 0x04 << 1,
    DS1302_DAY          = 0x05 << 1,
    DS1302_YEAR	        = 0x06 << 1,
    DS1302_CONTROL      = 0x07 << 1,
    DS1302_CHARGER      = 0x08 << 1,
    DS1302_BURST        = 0x1f << 1,
    DS1302_RAM          = 0x40,
    DS1302_READ         = 0x81,
    DS1302_WRITE        = 0x80,
};

/************************************************************/
/*      Mnemonic        value       meaning/usage           */

#define DS1302_CTL_WP   0x80      // the write-protect bit

#endif