/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_IIR_GX6605S_H_
#define _DRIVER_IIR_GX6605S_H_

#include <bits.h>

enum gx6605s_iir_registers {
    GX6605S_IIR_CTRL    = 0x00,
    GX6605S_IIR_INT     = 0x04,
    GX6605S_IIR_FIFO    = 0x08,
    GX6605S_IIR_CLK     = 0x0c,
};

/************************************************************************************/
/*      Mnemonic                    value       meaning/usage                       */

#define GX6605S_IIR_CTRL_POL        BIT(5)
#define GX6605S_IIR_CTRL_EN         BIT(3)
#define GX6605S_IIR_CTRL_FIFOTM     BIT(1)

#define GX6605S_IIR_CLKDIV(IN, HZ)  ((IN) / (HZ) - 1)

#endif  /* _DRIVER_IR_GX6605S_H_ */
