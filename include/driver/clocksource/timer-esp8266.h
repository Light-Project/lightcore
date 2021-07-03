#ifndef _DRIVER_TIMER_ESP8266_H_
#define _DRIVER_TIMER_ESP8266_H_

#include <bits.h>

/**
 * ESP8266 FRC is a 24-bit countdown timer
 */

enum TIMER_ESP8266_Registers
{
    TIMER_ESP8266_LOAD  = 0x00, // load value into the counter
    TIMER_ESP8266_COUNT = 0x04, // current value of the counter
    TIMER_ESP8266_CTRL  = 0x08,
    TIMER_ESP8266_INT   = 0x0c,
}; 


/****************************************************************************************/
/*      Mnemonic                    value        meaning/usage                          */

/* CTRL bitflag definitions */
#define TIMER_ESP8266_CTRL_INT      BIT(8)      // the status of the interrupt when the count is dereased to zero
#define TIMER_ESP8266_CTRL_EN       BIT(7)      // timer enable
#define TIMER_ESP8266_CTRL_AUTOLOAD BIT(6)      // automatically reload, when the counter is equal to zero
#define TIMER_ESP8266_CTRL_DIV_MSK  0x0c        // Prescale-divider mask
#define TIMER_ESP8266_CTRL_DIV_256  0x08        // divided by 256
#define TIMER_ESP8266_CTRL_DIV_16   0x04        // divided by 16
#define TIMER_ESP8266_CTRL_DIV_1    0x00        // divided by 1
#define TIMER_ESP8266_CTRL_TYPE     BIT(0)      // interrupt type, 0:edge, 1:level

/* INT bitflag definitions */
#define TIMER_ESP8266_INT_CLR       BIT(0)

#endif
