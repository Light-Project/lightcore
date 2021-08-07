#ifndef _DRIVER_CLOCKSOURCT_GX6605S_H_ 
#define _DRIVER_CLOCKSOURCT_GX6605S_H_

#include <bits.h>

enum TIMER_Registers
{
    TIMER_STATUS    = 0x00,
    TIMER_VALUE     = 0x04,
    TIMER_CONTRL    = 0x10,
    TIMER_CONFIG    = 0x20,
    TIMER_DIV       = 0x24,
    TIMER_INI       = 0x28,
};

#define TIMER_STATUS_CLR        BIT(0)

#define TIMER_CONTRL_RST        BIT(0)
#define TIMER_CONTRL_START      BIT(1)

#define TIMER_CONFIG_EN         BIT(0)
#define TIMER_CONFIG_IRQ_EN     BIT(1)

#endif  /* _DRIVER_CLOCKSOURCT_GX6605S_H_ */
