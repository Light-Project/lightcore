/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_USB_EHCI_H_
#define _DRIVER_USB_EHCI_H_

#include <bits.h>

/*
 * See EHCI Specification Section 2.3
 */
enum ehci_registers {
    EHCI_CMD            = 0x00, // USB Command
    EHCI_STS            = 0x04, // USB Status
    EHCI_INTR           = 0x08, // USB Interrupt Enable
    EHCI_FRNUM          = 0x0c, // USB Frame Index
    EHCI_SEGMENT        = 0x10, // 4G Segment Selector
    EHCI_FRAME          = 0x14, // Frame List Base Address
    EHCI_ASNEXT         = 0x18, // Next Asynchronous List Address
    EHCI_CONF           = 0x40, // Configured Flag Register
    EHCI_PORTSC         = 0x44, // Port Status/Control
};

enum ehci_qtd {
    EHCI_QTD_NEXT       = 0x00, /* next qtd pointe */
    EHCI_QTD       = 0x00, /* alternate next qtd pointer */
};

/******************************************************************************/
/*      Mnemonic            value        meaning/usage                        */

/*
 * The Command Register indicates the command to be executed by the serial bus host
 * controller. Writing to the register causes a command to be executed.
 */
#define EHCI_CMD_HIRD           (0xf<<24)   // host initiated resume duration
#define EHCI_CMD_PPCEE          BIT(15)     // per port change event enable
#define EHCI_CMD_FSP            BIT(14)     // fully synchronized prefetch
#define EHCI_CMD_ASPE           BIT(13)     // async schedule prefetch enable
#define EHCI_CMD_PSPE           BIT(12)     // periodic schedule prefetch enable
#define EHCI_CMD_PARK           BIT(11)     // enable "park" on async qh
#define EHCI_CMD_LRESET         BIT(7)      // partial reset (no ports, etc)
#define EHCI_CMD_IAAD           BIT(6)      // "doorbell" interrupt async advance
#define EHCI_CMD_ASE            BIT(5)      // async schedule enable
#define EHCI_CMD_PSE            BIT(4)      // periodic schedule enable
#define EHCI_CMD_RESET          BIT(1)      // reset HC not bus
#define EHCI_CMD_RUN            BIT(0)      // start/stop HC

/*
 * This register indicates pending interrupts and various states of the Host Controller.
 * The status resulting from a transaction on the serial bus is not indicated in this register.
 * Software sets a bit to 0 in this register by writing a 1 to it. See Section 4.15 for
 * additional information concerning USB interrupt conditions.
 */
#define EHCI_STS_PPCE           (0xff<<16)  // Per-Port change event 1-16
#define EHCI_STS_ASS            BIT(15)     // Async Schedule Status
#define EHCI_STS_PSS            BIT(14)     // Periodic Schedule Status
#define EHCI_STS_RECL           BIT(13)     // Reclamation
#define EHCI_STS_HALT           BIT(12)     // Not running (any reason)
#define EHCI_STS_IAA            BIT(5)      // Interrupted on async advance
#define EHCI_STS_FATAL          BIT(4)      // such as some PCI access errors
#define EHCI_STS_FLR            BIT(3)      // frame list rolled over
#define EHCI_STS_PCD            BIT(2)      // port change detect
#define EHCI_STS_ERR            BIT(1)      // "error" completion (overflow, ...)
#define EHCI_STS_INT            BIT(0)      // "normal" completion (short, ...)

/*
 * This register is used by the host controller to index into the periodic frame list.
 * The register updates every 125 microseconds (once each micro-frame).
 */
#define EHCI_FRNUM_INDEX        (0x1ff<<0)

/*
 * This 32-bit register contains the address of the next asynchronous queue head to be executed
 */

/*
 * This register enables and disables reporting of the corresponding interrupt to the software.
 * When a bit is set and the corresponding interrupt is active, an interrupt is generated to the host.
 * Interrupt sources that are disabled in this register still appear in the USBSTS to
 * allow the software to poll for events.
 */

/*
 *This register is in the auxiliary power well. It is only reset by hardware when the auxiliary power
 *is initially applied or in response to a host controller reset.
 */
#define EHCI_PORTSC_DEV_ADDR    (0x7f<<25)  // device address
#define EHCI_PORTSC_SSTS        (0x3<<23)   // suspend status
#define EHCI_PORTSC_WKOC_E      BIT(22)     // wake on overcurrent
#define EHCI_PORTSC_WKDISC_E    BIT(21)     // wake on disconnect
#define EHCI_PORTSC_WKCONN_E    BIT(20)     // wake on connect
#define EHCI_PORTSC_TEST        (0xf<<16)   // Port Test Control
#define EHCI_PORTSC_LED_MASK    (3<<14)
#define EHCI_PORTSC_LED_GREEN   (2<<14)
#define EHCI_PORTSC_LED_AMBER   (1<<14)
#define EHCI_PORTSC_LED_OFF     (0<<14)
#define EHCI_PORTSC_OWNER       BIT(13)     // companion hc owns this port
#define EHCI_PORTSC_POWER       BIT(12)     // has power (see PPC)
#define EHCI_PORTSC_LS_MASK     (3<<10)     // Link status (SE0, K or J)
#define EHCI_PORTSC_LPM         BIT(9)      // LPM transaction
#define EHCI_PORTSC_RESET       BIT(8)      // reset port
#define EHCI_PORTSC_SUSPEND     BIT(7)      // suspend port
#define EHCI_PORTSC_RESUME      BIT(6)      // resume it
#define EHCI_PORTSC_OCC         BIT(5)      // over current change
#define EHCI_PORTSC_OC          BIT(4)      // over current active
#define EHCI_PORTSC_PEC         BIT(3)      // port enable change
#define EHCI_PORTSC_PE          BIT(2)      // port enable
#define EHCI_PORTSC_CSC         BIT(1)      // connect status change
#define EHCI_PORTSC_CONNECT     BIT(0)      // device connected

#define EHCI_MODE_SDIS          BIT(3)      // Stream disable
#define EHCI_MODE_BE            BIT(2)      // BE/LE endianness select
#define EHCI_MODE_CM_HC         (3<<0)      // host controller mode
#define EHCI_MODE_CM_IDLE	    (0<<0)      // idle state

#define EHCI_HOSTPC_PSPD        (3<<25)     // Port speed detection
#define EHCI_HOSTPC_PHCD        BIT(22)     // Phy clock disable

#endif /* _DRIVER_USB_EHCI_H_ */
