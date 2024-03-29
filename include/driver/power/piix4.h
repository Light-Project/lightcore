/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_POWER_PIIX4_H_
#define _DRIVER_POWER_PIIX4_H_

#include <bits.h>

enum piix4_power_registers {
    PIIX4_POWER_STS         = 0x00, /* (RW) POWER MANAGEMENT STATUS REGISTER */
    PIIX4_POWER_PMEN        = 0x02, /* (RW) POWER MANAGEMENT RESUME ENABLE REGISTER */
    PIIX4_POWER_CNTRL       = 0x04, /* (RW) POWER MANAGEMENT CONTROL REGISTER */
    PIIX4_POWER_PMTMR       = 0x08, /* (RO) POWER MANAGEMENT TIMER REGISTER */
    PIIX4_POWER_GPSTS       = 0x0c, /* (RW) GENERAL PURPOSE STATUS REGISTER */
    PIIX4_POWER_GPEN        = 0x0e, /* (RW) GENERAL PURPOSE ENABLE REGISTER */
    PIIX4_POWER_PCNTRL      = 0x10, /* (RW) PROCESSOR CONTROL REGISTER */
    PIIX4_POWER_PLVL2       = 0x14, /* (RW) PROCESSOR LEVEL 2 REGISTER */
    PIIX4_POWER_PLVL3       = 0x15, /* (RW) PROCESSOR LEVEL 3 REGISTER */
    PIIX4_POWER_GLBSTS      = 0x18, /* (RW) GLOBAL STATUS REGISTER */
    PIIX4_POWER_DEVSTS      = 0x1c, /* (RW) DEVICE STATUS REGISTER */
    PIIX4_POWER_GLBEN       = 0x20, /* (RW) GLOBAL ENABLE REGISTER */
    PIIX4_POWER_GLBCTL      = 0x28, /* (RW) GLOBAL CONTROL REGISTER */
    PIIX4_POWER_DEVCTL      = 0x2c, /* (RW) DEVICE CONTROL REGISTER */
    PIIX4_POWER_GPI1        = 0x30, /* (RO) GENERAL PURPOSE INPUT REGISTER */
    PIIX4_POWER_GPI2        = 0x31, /* (RO) GENERAL PURPOSE INPUT REGISTER */
    PIIX4_POWER_GPI3        = 0x32, /* (RO) GENERAL PURPOSE INPUT REGISTER */
    PIIX4_POWER_GPO1        = 0x34, /* (RW) GENERAL PURPOSE OUTPUT REGISTER */
    PIIX4_POWER_GPO2        = 0x35, /* (RW) GENERAL PURPOSE OUTPUT REGISTER */
    PIIX4_POWER_GPO3        = 0x36, /* (RW) GENERAL PURPOSE OUTPUT REGISTER */
    PIIX4_POWER_GPO4        = 0x37, /* (RW) GENERAL PURPOSE OUTPUT REGISTER */
};

#define PIIX4_POWER_STS_RSM         BIT(15)
#define PIIX4_POWER_STS_PWRBTNOR    BIT(11)
#define PIIX4_POWER_STS_RTC         BIT(10)
#define PIIX4_POWER_STS_PWRBTN      BIT(8)
#define PIIX4_POWER_STS_GBL         BIT(5)
#define PIIX4_POWER_STS_BM          BIT(4)
#define PIIX4_POWER_STS_TMROF       BIT(0)

#define PIIX4_POWER_PMEN_RTC        BIT(10)
#define PIIX4_POWER_PMEN_PWRBTN     BIT(8)
#define PIIX4_POWER_PMEN_GBL        BIT(5)
#define PIIX4_POWER_PMEN_TMROF      BIT(0)

#define PIIX4_POWER_CNTRL_SUS_EN    BIT(13)
#define PIIX4_POWER_CNTRL_SUS_TYPE  BIT_RANGE(12, 10)
#define PIIX4_POWER_CNTRL_GBL_RLS
#define PIIX4_POWER_CNTRL_SCI       BIT(0)

#endif /* _DRIVER_POWER_PIIX4_H_ */
