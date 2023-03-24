/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_UART_GX6605S_H_
#define _DRIVER_UART_GX6605S_H_

#include <bits.h>

enum gx6605s_uart_registers {
    GX6605S_UART_CTL    = 0x00,
    GX6605S_UART_STA    = 0x04,
    GX6605S_UART_DAT    = 0x08,
    GX6605S_UART_CLK    = 0x0c,
    GX6605S_UART_CFG    = 0x10,
};

/********************************************************************************************/
/*      Mnemonic                    value           meaning/usage                           */

#define GX6605S_UART_STA_THRE       BIT(7)      // register empty
#define GX6605S_UART_STA_DR         BIT(2)      // data ready

#endif /* _DRIVER_UART_GX6605S_H_ */
