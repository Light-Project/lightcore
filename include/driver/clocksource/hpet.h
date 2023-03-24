/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLOCKSOURCE_HPET_H_
#define _DRIVER_CLOCKSOURCE_HPET_H_

#include <bits.h>

enum hpet_registers {
    HPET_GCAP       = 0x0000, /* (RO) General Capabilities */
    HPET_PERIOD     = 0x0004, /* (RO) Main Counter Tick Period */
    HPET_GCONF      = 0x0010, /* (RW) General Configuration */
    HPET_GINTR      = 0x0020, /* (RC) General Interrupt Status */
    HPET_MAIN       = 0x00f0, /* (RW) Main Counter Value */
    HPET_CONF_BASE  = 0x0100, /* (RW) Timer Configuration */
    HPET_CAP_BASE   = 0x0104, /* (RO) Timer Capabilities */
    HPET_CMP_BASE   = 0x0108, /* (RW) Timer Comparator Value */
    HPET_ROUTE_BASE = 0x0110, /* (RW) Main Counter Value */
};

#define HPET_CFG(n)     (0x100 + 0x20 * n)
#define HPET_CMP(n)     (0x108 + 0x20 * n)
#define HPET_ROUTE(n)   (0x110 + 0x20 * n)

/************************************************************************************/
/*      Mnemonic                value                  meaning/usage                */

#define HPET_GCAP_VENDOR        BIT_RANGE(31, 16)   /* Vendor ID Capability */
#define HPET_GCAP_WIDE          BIT(13)             /* Counter is 64-bit Wide */
#define HPET_GCAP_NUMBER        BIT_RANGE(12, 8)    /* Number of Timer Capability */
#define HPET_GCAP_REVISION      BIT_RANGE(7, 0)     /* Revision Identification */

#define HPET_GCONF_LEGACY       BIT(1)              /* Legacy Replacement Rout */
#define HPET_GCONF_ENABLE       BIT(0)              /* Overall Enable */
#define HPET_GINTR_ACTIVE(tn)   BIT(n)              /* Timer Interrupt Active */

#define HPET_CONF_ROUTE         BIT_RANGE(13, 9)    /* Interrupt Rout */
#define HPET_CONF_32BIT         BIT(8)              /* Timer in 32-bit Mode */
#define HPET_CONF_SETVAL		BIT(6)              /* Timer Value Set */
#define HPET_CONF_64BIT_CAP     BIT(5)              /* Timer 64-bit Mode Capable */
#define HPET_CONF_PERIODIC_CAP  BIT(4)              /* Periodic Interrupt Capable */
#define HPET_CONF_PERIODIC      BIT(3)              /* Enable Timer Periodic Interrupt */
#define HPET_CONF_INT_ENABLE    BIT(2)              /* Timer Interrupt Enable */
#define HPET_CONF_INT_LEVEL     BIT_SHIFT(0, 1)     /* Timer Interrupt Type: Level Triggered */
#define HPET_CONF_INT_EDGE      BIT_SHIFT(0, 0)     /* Timer Interrupt Type: Edge Triggered */

#endif /* _DRIVER_CLOCKSOURCE_HPET_H_ */
