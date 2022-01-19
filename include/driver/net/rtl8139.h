/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_NET_RTL8139_H_
#define _DRIVER_NET_RTL8139_H_

#include <bits.h>

enum rtl8139_registers {
    RTL8139_MAC0            = 0x00,
    RTL8139_MAR0            = 0x08,
    RTL8139_TXSTATUS0       = 0x10,
    RTL8139_TXADDR0         = 0x20,
    RTL8139_RXBUF           = 0x30,
    RTL8139_CHIPCMD         = 0x37,
    RTL8139_RXBUFPTR        = 0x38,
    RTL8139_RXBUFADDR       = 0x3a,
    RTL8139_INTRMASK        = 0x3c,
    RTL8139_INTRSTATUS      = 0x3e,
    RTL8139_TXCONFIG        = 0x40,
    RTL8139_RXCONFIG        = 0x44,
    RTL8139_TIMER           = 0x48,
    RTL8139_RXMISSED        = 0x4c,
    RTL8139_CFG9346         = 0x50,
    RTL8139_CONFIG0         = 0x51,
    RTL8139_CONFIG1         = 0x52,
    RTL8139_TIMERINT        = 0x54,
    RTL8139_MEDIASTATUS     = 0x58,
    RTL8139_CONFIG3         = 0x59,
    RTL8139_CONFIG4         = 0x5a,
    RTL8139_HLTCLK          = 0x5b,
    RTL8139_MULTIINTR       = 0x5c,
    RTL8139_TXSUMMARY       = 0x60,
    RTL8139_BASICMODECTRL   = 0x62,
    RTL8139_BASICMODESTATUS = 0x64,
    RTL8139_NWAYADVERT      = 0x66,
    RTL8139_NWAYLPAR        = 0x68,
    RTL8139_NWAYEXPANSION   = 0x6a,
    RTL8139_FIFOTMS         = 0x70,
    RTL8139_CSCR            = 0x74,
    RTL8139_PARA78          = 0x78,
    RTL8139_FLASHREG        = 0xd4,
    RTL8139_PARA7C          = 0x7c,
    RTL8139_CONFIG5         = 0xd8,
};

/************************************************************************************/
/*      Mnemonic                    value               meaning/usage               */

#define RTL8139_CHIPCMD_RESET       BIT(4)

#define RTL8139_CFG9346_UNLOCK      (0xc0)
#define RTL8139_CFG9346_LOCK        (0x00)

#define RTL8139_CONFIG1_LED1        BIT(7)
#define RTL8139_CONFIG1_LED0        BIT(6)
#define RTL8139_CONFIG1_DRIVER_LOAD BIT(5)
#define RTL8139_CONFIG1_LWAKE       BIT(4)
#define RTL8139_CONFIG1_MMIO        BIT(3)
#define RTL8139_CONFIG1_PIO         BIT(2)
#define RTL8139_CONFIG1_VPD_ENABLE  BIT(1)
#define RTL8139_CONFIG1_PM_ENABLE   BIT(0)
#define RTL8139_CONFIG1_SLEEP       BIT(1)              /* (only 8139/8139A) */
#define RTL8139_CONFIG1_PWRDN       BIT(0)              /* (only 8139/8139A) */

#define RTL8139_CONFIG4_LWPTN       BIT(2)              /* (none 8139/8139A) */

#endif  /* _DRIVER_NET_RTL8139_H_ */
