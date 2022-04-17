/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PARALLEL_I8255_H_
#define _DRIVER_PARALLEL_I8255_H_

#include <bits.h>

enum i8255_registers {
    I8255_DATA          = 0x00,  /* (WO) Data Port */
    I8255_STATUS        = 0x01,  /* (RO) Status Port */
    I8255_CTRL          = 0x02,  /* (WO) Control Port */
};

/************************************************************************************/
/*      Mnemonic                value       meaning/usage                           */

/*
 * The ERROR, ACK and BUSY signals are active
 * low when reading from the IO port.
 */

#define I8255_STATUS_BUSY       BIT(7)
#define I8255_STATUS_ACK        BIT(6)
#define I8255_STATUS_PAPEROUT   BIT(5)
#define I8255_STATUS_SELECT     BIT(4)
#define I8255_STATUS_ERROR      BIT(3)

#define I8255_CTRL_SELECT       BIT(3)
#define I8255_CTRL_INIT         BIT(2)
#define I8255_CTRL_AUTOLF       BIT(1)
#define I8255_CTRL_STROBE       BIT(0)

#endif  /* _DRIVER_PARALLEL_I8255_H_ */
