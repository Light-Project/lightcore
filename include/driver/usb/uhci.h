/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_USB_UHCI_H_
#define _DRIVER_USB_UHCI_H_

#include <bits.h>

enum uhci_registers {
    UHCI_CMD            = 0x00, /* Command register */
    UHCI_STS            = 0x02, /* Status register */
    UHCI_INTR           = 0x04, /* Interrupt enable register */
    UHCI_FRNUM          = 0x06,
    UHCI_FRFLBASEADD    = 0x08,
    UHCI_SOF            = 0x0c,
    UHCI_STCTL1	        = 0x10, /* USB port1 status control register */
    UHCI_STCTL2	        = 0x12, /* USB port2 status control register */
    UHCI_STCTL3	        = 0x14, /* USB port3 status control register */
    UHCI_STCTL4	        = 0x16, /* USB port4 status control register */
};

enum uhci_pci_config {
    UHCI_PCI_LEGSUP     = 0xc0, /* PCI legacy support register */
    UHCI_PCI_RESUME     = 0xc4, /* PCI Intel-specific resume-enable register */
};

/******************************************************************************/
/*      Mnemonic            value        meaning/usage                        */

#define UHCI_CMD_RUN        BIT(0)  /* Run/Stop */
#define UHCI_CMD_HCRESET    BIT(1)  /* Host reset */
#define UHCI_CMD_GRESET     BIT(2)  /* Global reset */
#define UHCI_CMD_EGSM       BIT(3)  /* Global Suspend Mode */
#define UHCI_CMD_FGR        BIT(4)  /* Force Global Resume */
#define UHCI_CMD_SWDBG      BIT(5)  /* SW Debug mode */
#define UHCI_CMD_CF         BIT(6)  /* Config Flag (sw only) */
#define UHCI_CMD_MAXP       BIT(7)  /* Max Packet (0 = 32, 1 = 64) */

#define UHCI_STS_USBINT     BIT(0)  /* Interrupt due to IOC */
#define UHCI_STS_ERROR      BIT(1)  /* Interrupt due to error */
#define UHCI_STS_RD         BIT(2)  /* Resume Detect */
#define UHCI_STS_HSE		BIT(3)  /* Host System Error */
#define UHCI_STS_HCPE		BIT(4)  /* Host Controller Process Error */
#define UHCI_STS_HCH		BIT(5)  /* HC Halted */

#define UHCI_INTR_TIMEOUT   BIT(0)  /* Timeout/CRC error enable */
#define UHCI_INTR_RESUME    BIT(1)  /* Resume interrupt enable */
#define UHCI_INTR_IOC       BIT(2)  /* Interrupt On Complete enable */
#define UHCI_INTR_SP        BIT(3)  /* Short packet interrupt enable */

#define UHCI_PORTSC_CCS     BIT(0)  /* Current Connect Status */
#define UHCI_PORTSC_CSC     BIT(1)  /* Connect Status Change */
#define UHCI_PORTSC_PE      BIT(2)  /* Port Enable */
#define UHCI_PORTSC_PEC     BIT(3)  /* Port Enable Change */
#define UHCI_PORTSC_DPLUS   BIT(4)  /* D+ high (line status) */
#define UHCI_PORTSC_DMINUS  BIT(5)  /* D- high (line status) */
#define UHCI_PORTSC_RD      BIT(6)  /* Resume Detect */
#define UHCI_PORTSC_RES1    BIT(7)  /* reserved, always 1 */
#define UHCI_PORTSC_LSDA    BIT(8)  /* Low Speed Device Attached */
#define UHCI_PORTSC_PR      BIT(9)  /* Port Reset */
#define UHCI_PORTSC_OC      BIT(10) /* Over Current condition */
#define UHCI_PORTSC_OCC     BIT(11) /* Over Current Change R/WC */
#define UHCI_PORTSC_SUSP    BIT(12) /* Suspend */
#define UHCI_PORTSC_RES2    BIT(13) /* reserved, write zeroes */
#define UHCI_PORTSC_RES3    BIT(14) /* reserved, write zeroes */
#define UHCI_PORTSC_RES4    BIT(15) /* reserved, write zeroes */

#define UHCI_PORT1EN        BIT(0)
#define UHCI_PORT2EN        BIT(1)

#define UHCI_PCI_LEGSUP_RWC  0x8f00  /* the R/WC bits */
#define UHCI_PCI_LEGSUP_RO   0x5040  /* R/O and reserved bits */

#endif  /* _DRIVER_USB_UHCI_H_ */
