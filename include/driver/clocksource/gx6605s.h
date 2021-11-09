#ifndef _DRIVER_CLOCKSOURCT_GX6605S_H_
#define _DRIVER_CLOCKSOURCT_GX6605S_H_

#include <bits.h>

enum gx6605s_timer_registers {
    GX6605S_TIM_STATUS      = 0x00,
    GX6605S_TIM_VALUE       = 0x04,
    GX6605S_TIM_CONTRL      = 0x10,
    GX6605S_TIM_CONFIG      = 0x20,
    GX6605S_TIM_CLKDIV      = 0x24,
    GX6605S_TIM_RELOAD      = 0x28,
};

/*************************************************************************************/
/*      Mnemonic                    value     meaning/usage                          */

#define GX6605S_TIM_STATUS_CLR      BIT(0)

#define GX6605S_TIM_CONTRL_START    BIT(1)
#define GX6605S_TIM_CONTRL_RST      BIT(0)

#define GX6605S_TIM_CONFIG_IRQ_EN   BIT(1)
#define GX6605S_TIM_CONFIG_EN       BIT(0)

#define GX6605S_TIM_LATCH(fin, hz)  (((fin) / (hz)) - 1)

#endif  /* _DRIVER_CLOCKSOURCT_GX6605S_H_ */
