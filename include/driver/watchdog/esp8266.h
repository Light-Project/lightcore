/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_WATCHDOG_ESP8266_H_
#define _DRIVER_WATCHDOG_ESP8266_H_

#include <bits.h>

enum esp8266_wdt_registers {
    ESP8266_WDT_CTL     = 0x00, // Control register
    ESP8266_WDT_OP      = 0x04, // Reset time
    ESP8266_WDT_OPND    = 0x08, // Interruption time
    ESP8266_WDT_RST     = 0x14, // Feed register
};

/* OP & OPND bitflag definitions */
/* wdt freq = osc / 32000, osc = 40m -> (1 / 40m) = 0.8ms */
/* 2^n * 0.8ms, mask 0xf, n = 13 -> (2^13 = 8192) * 0.8 * 0.001 = 6.5536 */

/********************************************************************************************/
/*      Mnemonic                        value           meaning/usage                       */

#define ESP8266_WDT_CTL_RSTLEN_MASK     BIT_RANGE(5, 3)
#define ESP8266_WDT_CTL_RSPMOD_MASK     BIT_RANGE(2, 1)
#define ESP8266_WDT_CTL_EN              BIT(0)

#define ESP8266_WDT_RST_FEED_MASK       0x73

#endif  /* _DRIVER_WATCHDOG_ESP8266_H_ */
