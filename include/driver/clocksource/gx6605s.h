#ifndef _DRIVER_CLOCKSOURCT_GX6605S_H_
#define _DRIVER_CLOCKSOURCT_GX6605S_H_

#include <bits.h>

enum timer_registers
{
    GX6605S_TIM_STATUS  = 0x00,
    GX6605S_TIM_VALUE   = 0x04,
    GX6605S_TIM_CONTRL  = 0x10,
    GX6605S_TIM_CONFIG  = 0x20,
    GX6605S_TIM_DIV     = 0x24,
    GX6605S_TIM_INI     = 0x28,
};

#define TIMER_STATUS_CLR        BIT(0)

#define TIMER_CONTRL_RST        BIT(0)
#define TIMER_CONTRL_START      BIT(1)

#define TIMER_CONFIG_EN         BIT(0)
#define TIMER_CONFIG_IRQ_EN     BIT(1)

#endif  /* _DRIVER_CLOCKSOURCT_GX6605S_H_ */
