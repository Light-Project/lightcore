/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_TIMER_ESP8266_H_
#define _DRIVER_TIMER_ESP8266_H_

#include <bits.h>

/**
 * ESP8266 FRC is a 24-bit countdown timer
 */

enum esp8266_timer_registers {
    ESP8266_TIMER_LOAD  = 0x00, // load value into the counter
    ESP8266_TIMER_COUNT = 0x04, // current value of the counter
    ESP8266_TIMER_CTRL  = 0x08,
    ESP8266_TIMER_INT   = 0x0c,
};

/********************************************************************************************/
/*      Mnemonic                        value        meaning/usage                          */

#define ESP8266_TIMER_CTRL_INT          BIT(8)      // the status of the interrupt when the count is dereased to zero
#define ESP8266_TIMER_CTRL_EN           BIT(7)      // timer enable
#define ESP8266_TIMER_CTRL_AUTOLOAD     BIT(6)      // automatically reload, when the counter is equal to zero
#define ESP8266_TIMER_CTRL_DIV_MSK      0x0c        // Prescale-divider mask
#define ESP8266_TIMER_CTRL_DIV_256      0x08        // divided by 256
#define ESP8266_TIMER_CTRL_DIV_16       0x04        // divided by 16
#define ESP8266_TIMER_CTRL_DIV_1        0x00        // divided by 1
#define ESP8266_TIMER_CTRL_TYPE         BIT(0)      // interrupt type, 0:edge, 1:level

#define ESP8266_TIMER_INT_CLR           BIT(0)

#endif /* _DRIVER_TIMER_ESP8266_H_ */
