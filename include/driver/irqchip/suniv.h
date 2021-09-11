/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_IRQCHIP_SUNIV_H_
#define _DRIVER_IRQCHIP_SUNIV_H_

enum suniv_intc_registers {
    SUNIV_INTC_VECTOR   = 0x00,
    SUNIV_INTC_BASE     = 0x04,
    SUNIV_INTC_NMI_CTL  = 0x0c,
    SUNIV_INTC_PEND0    = 0x10,
    SUNIV_INTC_PEND1    = 0x14,
    SUNIV_INTC_EN0      = 0x20,
    SUNIV_INTC_EN1      = 0x24,
    SUNIV_INTC_MASK0    = 0x30,
    SUNIV_INTC_MASK1    = 0x34,
    SUNIV_INTC_RESP0    = 0x40,
    SUNIV_INTC_RESP1    = 0x44,
    SUNIV_INTC_FF0      = 0x50,
    SUNIV_INTC_FF1      = 0x54,
    SUNIV_INTC_PRIO0    = 0x60,
    SUNIV_INTC_PRIO1    = 0x64,
    SUNIV_INTC_PRIO2    = 0x68,
    SUNIV_INTC_PRIO3    = 0x6c,
};

/********************************************************************/
/*      Mnemonic            value    meaning/usage                  */

#endif  /* _DRIVER_IRQCHIP_SUNIV_H_ */
