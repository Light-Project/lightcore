/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_NET_RTL8139_H_
#define _DRIVER_NET_RTL8139_H_

#include <bits.h>

enum rtl8139_registers {
    RTL8139_MAC0            = 0x00, /* RW: Ethernet hardware address */
    RTL8139_MAR0            = 0x08, /* RW: Multicast filter */
    RTL8139_TXSTATUS0       = 0x10, /* RW: Transmit status (Four 32bit registers) */
    RTL8139_TXADDR0         = 0x20, /* RW: Tx descriptors (also four 32bit) */
    RTL8139_RXBUF           = 0x30, /* RW: Receive (Rx) Buffer Start Address */
    RTL8139_CHIPCMD         = 0x37, /* RW: Command Register */
    RTL8139_RXBUFPTR        = 0x38, /* RW: Current Address of Packet Read */
    RTL8139_RXBUFADDR       = 0x3a, /* RW: Current Buffer Address */
    RTL8139_INTRMASK        = 0x3c, /* RW: Interrupt Mask Register */
    RTL8139_INTRSTATUS      = 0x3e, /* RW: Interrupt Status Register */
    RTL8139_TXCONFIG        = 0x40, /* RW: Transmit (Tx) Configuration Register */
    RTL8139_RXCONFIG        = 0x44, /* RW: Receive (Rx) Configuration Register */
    RTL8139_TIMER           = 0x48, /* RW: A general-purpose counter */
    RTL8139_RXMISSED        = 0x4c, /* RW: 24 bits valid, write clears */
    RTL8139_CFG9346         = 0x50, /* RW: 93C46 Command Register */
    RTL8139_CONFIG0         = 0x51, /* RW: Configuration Register 0 */
    RTL8139_CONFIG1         = 0x52, /* RW: Configuration Register 1 */
    RTL8139_TIMERINT        = 0x54, /* RW: Timer Interrupt Register */
    RTL8139_MEDIASTATUS     = 0x58, /* RW: Media Status Register */
    RTL8139_CONFIG3         = 0x59, /* RW: Configuration register 3 */
    RTL8139_CONFIG4         = 0x5a, /* RW: @ Configuration register 4 */
    RTL8139_HLTCLK          = 0x5b, /* RW:  */
    RTL8139_MULTIINTR       = 0x5c, /* RW: Multiple Interrupt Select */
    RTL8139_TXSUMMARY       = 0x60, /* RO: Transmit Status of All Descriptors */
    RTL8139_BASICMODECTRL   = 0x62, /* RW: Basic Mode Control Register */
    RTL8139_BASICMODESTATUS = 0x64, /* RW: Basic Mode Status Register */
    RTL8139_NWAYADVERT      = 0x66, /* RW: Auto-Negotiation Advertisement Register */
    RTL8139_NWAYLPAR        = 0x68, /* RW: Auto-Negotiation Link Partner Register */
    RTL8139_NWAYEXPANSION   = 0x6a, /* RW: Auto-Negotiation Expansion Register */

    RTL8139_FIFOTMS         = 0x70, /* RW: FIFO Control and test */
    RTL8139_CSCR            = 0x74, /* RW: Chip Status and Configuration Register */
    RTL8139_PARA78          = 0x78, /* RW:  */
    RTL8139_FLASHREG        = 0xd4, /* RW: Communication with Flash ROM, four bytes */
    RTL8139_PARA7C          = 0x7c, /* RW: Magic transceiver parameter register */
    RTL8139_CONFIG5         = 0xd8, /* RW: @ Configuration register 5 */
};

/************************************************************************************/
/*      Mnemonic                        value           meaning/usage               */

#define RTL8139_CHIPCMD_RESET           BIT(4)

#define RTL8139_CFG9346_UNLOCK          (0xc0)
#define RTL8139_CFG9346_LOCK            (0x00)

#define RTL8139_CONFIG1_LED1            BIT(7)
#define RTL8139_CONFIG1_LED0            BIT(6)
#define RTL8139_CONFIG1_DRIVER_LOAD     BIT(5)
#define RTL8139_CONFIG1_LWAKE           BIT(4)
#define RTL8139_CONFIG1_MMIO            BIT(3)
#define RTL8139_CONFIG1_PIO             BIT(2)
#define RTL8139_CONFIG1_VPD_ENABLE      BIT(1)
#define RTL8139_CONFIG1_PM_ENABLE       BIT(0)
#define RTL8139_CONFIG1_SLEEP           BIT(1)
#define RTL8139_CONFIG1_PWRDN           BIT(0)

#define RTL8139_CONFIG3_GNTSEL          BIT(7)      /* delay 1 clock from PCI GNT signal */
#define RTL8139_CONFIG3_PARM_EN         BIT(6)      /* software can set twister parameters */
#define RTL8139_CONFIG3_MAGIC           BIT(5)      /* wake up on Magic Packet (tm) */
#define RTL8139_CONFIG3_LINKUP          BIT(4)      /* wake up on link up */
#define RTL8139_CONFIG3_CARDB_EN        BIT(3)      /* enable CardBus registers */
#define RTL8139_CONFIG3_CLKRUN_EN       BIT(2)      /* enable CLKRUN */
#define RTL8139_CONFIG3_FUNCREGEN       BIT(1)      /* enable CardBus Function registers */
#define RTL8139_CONFIG3_FBTBEN          BIT(0)      /* Fast Back to Back */

#define RTL8139_CONFIG4_LWPTN           BIT(2)

#endif  /* _DRIVER_NET_RTL8139_H_ */
