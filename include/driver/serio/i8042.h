/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PS2_i8042_H_
#define _DRIVER_PS2_i8042_H_

#include <bits.h>
#include <driver/serio/ps2.h>

enum i8042_registers {
    I8042_DATA              = 0x00, /* (RW) Data Port */
    I8042_STATUS            = 0x04, /* (RO) Status Register */
    I8042_CMD               = 0x04, /* (WO) Command Register */
};

enum i8042_cmd {
    I8042_CMD_CTL_RCTR      = PS2_CMD(1, 0, 0x20),
    I8042_CMD_CTL_WCTR      = PS2_CMD(0, 1, 0x60),
    I8042_CMD_CTL_VERSION   = 0,
    I8042_CMD_CTL_TEST      = PS2_CMD(1, 0, 0xaa),

    I8042_CMD_PORT_INPUT    = PS2_CMD(1, 0, 0xd0),
    I8042_CMD_PORT_OUTPUT   = PS2_CMD(1, 0, 0xd1),
    I8042_CMD_PORT_RESET    = PS2_CMD(0, 0, 0xf0),
    I8042_CMD_PORT_PULSE    = PS2_CMD(0, 0, 0xfe),
    I8042_CMD_PORT_NOOP     = PS2_CMD(0, 0, 0xff),

    I8042_CMD_KBD_DISABLE   = PS2_CMD(0, 0, 0xad),
    I8042_CMD_KBD_ENABLE    = PS2_CMD(0, 0, 0xae),
    I8042_CMD_KBD_TEST      = PS2_CMD(1, 0, 0xab),
    I8042_CMD_KBD_LOOP      = PS2_CMD(1, 1, 0xd2),

    I8042_CMD_AUX_DISABLE   = PS2_CMD(0, 0, 0xa7),
    I8042_CMD_AUX_ENABLE    = PS2_CMD(0, 0, 0xa8),
    I8042_CMD_AUX_TEST      = PS2_CMD(1, 0, 0xa9),
    I8042_CMD_AUX_SEND      = PS2_CMD(0, 1, 0xd4),
    I8042_CMD_AUX_LOOP      = PS2_CMD(1, 1, 0xd3),

    I8042_CMD_MUX_PFX       = PS2_CMD(0, 0, 0x90),
    I8042_CMD_MUX_SEND      = PS2_CMD(0, 1, 0x90),
};

/*************************************************************************************/
/*      Mnemonic                    value     meaning/usage                          */

#define I8042_STATUS_PARITY         BIT(7)  /* Parity error (0: no error 1: parity error) */
#define I8042_STATUS_TIMEOUT        BIT(6)  /* Time-out error (0: no error 1: time-out error) */
#define I8042_STATUS_AUXDATA        BIT(5)  /* Chip Specific: Receive Time-out */
#define I8042_STATUS_KEYLOCK        BIT(4)  /* Chip Specific: Keyboard Inhibited */
#define I8042_STATUS_CMDDAT         BIT(3)  /* Command/data: (0: write data 2: write cmd) */
#define I8042_STATUS_SFLAG          BIT(2)  /* Meant to be cleared on reset and set by firmware */
#define I8042_STATUS_IBF            BIT(1)  /* Input Buffer Full */
#define I8042_STATUS_OBF            BIT(0)  /* Output Buffer Full */

#define I8042_CTRL_XLATE            BIT(6)  /* First PS/2 port translation */
#define I8042_CTRL_AUXDIS           BIT(5)  /* Second PS/2 port clock */
#define I8042_CTRL_KBDDIS           BIT(4)  /* First PS/2 port clock */
#define I8042_CTRL_IGNKEYLOCK       BIT(3)  /* Handle keylock */
#define I8042_CTRL_SYSTEM           BIT(2)  /* System Flag */
#define I8042_CTRL_AUXINT           BIT(1)  /* Second PS/2 port interrupt */
#define I8042_CTRL_KBDINT           BIT(0)  /* First PS/2 port interrupt */

#define I8042_PORT_AUX_OBF          BIT(3)
#define I8042_PORT_KBD_OBF          BIT(2)
#define I8042_PORT_A20              BIT(1)
#define I8042_PORT_RESET            BIT(0)

#endif /* _DRIVER_PS2_i8042_H_ */
