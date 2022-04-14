/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PARALLEL_I8255_H_
#define _DRIVER_PARALLEL_I8255_H_

#include <bits.h>

enum i8255_registers {
    I8255_DATA          = 0x00,
    I8255_STATUS        = 0x01,
    I8255_CTRL          = 0x02,
    I8255_EPPADDR       = 0x03,
    I8255_EPPDATA       = 0x04,
};

/************************************************************************************/
/*      Mnemonic                value       meaning/usage                           */

/*
 * The ERROR, ACK and BUSY signals are active
 * low when reading from the IO port.
 */

#define I8255_STATUS_BUSY       BIT(7)  /* Busy pin status */
#define I8255_STATUS_ACK        BIT(6)  /* Ack pin status */
#define I8255_STATUS_PAROUT     BIT(5)  /* Paper-out pin status */
#define I8255_STATUS_SELIN      BIT(4)  /* Select-in pin status */
#define I8255_STATUS_ERROR      BIT(3)  /* Error pin status */
#define I8255_STATUS_IRQ        BIT(2)  /* Interrupt status */

#define I8255_CTRL_BIDI         BIT(5)  /* Value */
#define I8255_CTRL_IRQACK       BIT(4)  /* Value */
#define I8255_CTRL_SELECT       BIT(3)  /* Select-pt Value */
#define I8255_CTRL_INIT         BIT(2)  /* Init pin Value */
#define I8255_CTRL_AUTOLF       BIT(1)  /* Autof Value */
#define I8255_CTRL_STROBE       BIT(0)  /* Value */

#endif  /* _DRIVER_PARALLEL_I8255_H_ */
