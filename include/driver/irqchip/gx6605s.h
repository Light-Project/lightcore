/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_IRQCHIP_GX6605S_H_
#define _DRIVER_IRQCHIP_GX6605S_H_

enum gx6605s_intc_registers {
    GX6605S_INTC_PEN31_00       = 0x00,
    GX6605S_INTC_PEN63_32       = 0x04,
    GX6605S_INTC_NEN31_00       = 0x40,
    GX6605S_INTC_NEN63_32       = 0x44,
    GX6605S_INTC_NMASK31_00     = 0x50,
    GX6605S_INTC_NMASK63_32     = 0x54,
    GX6605S_INTC_SOURCE         = 0x60,
};


#define GX6605S_IRQS     64

#endif  /* _DRIVER_IRQCHIP_GX6605S_H_ */
