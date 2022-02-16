/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PS2_i8042_H_
#define _DRIVER_PS2_i8042_H_

#include <bits.h>

enum i8042_registers {
    I8042_DATA              = 0x00, /* (RW) Data Port */
    I8042_STATUS            = 0x04, /* (RO) Status Register */
    I8042_CMD               = 0x04, /* (WO) Command Register */
};

enum i8042_cmd {
    I8042_CMD_CTL_RCTR      = 0x20 | (0x01 << 8),
    I8042_CMD_CTL_WCTR      = 0x60 | (0x10 << 8),
    I8042_CMD_CTL_TEST      = 0xaa | (0x01 << 8),

    I8042_CMD_PORT_INPUT    = 0xd0 | (0x01 << 8),
    I8042_CMD_PORT_OUTPUT   = 0xd1 | (0x01 << 12),

    I8042_CMD_KBD_DISABLE   = 0xad | (0x00 << 8),
    I8042_CMD_KBD_ENABLE    = 0xae | (0x00 << 8),
    I8042_CMD_KBD_TEST      = 0xab | (0x01 << 8),
    I8042_CMD_KBD_LOOP      = 0xd2 | (0x11 << 8),

    I8042_CMD_AUX_DISABLE   = 0xa7 | (0x00 << 8),
    I8042_CMD_AUX_ENABLE    = 0xa8 | (0x00 << 8),
    I8042_CMD_AUX_TEST      = 0xa9 | (0x01 << 8),
    I8042_CMD_AUX_SEND      = 0xd4 | (0x10 << 8),
    I8042_CMD_AUX_LOOP      = 0xd3 | (0x11 << 8),

    I8042_CMD_MUX_PFX       = 0x90 | (0x00 << 8),
    I8042_CMD_MUX_SEND      = 0x90 | (0x10 << 8),
};

/*************************************************************************************/
/*      Mnemonic                    value     meaning/usage                          */

#define I8042_STATUS_PARITY         BIT(7)  /* Parity error (0: no error 1: parity error) */
#define I8042_STATUS_TIMEOUT        BIT(6)  /* Time-out error (0: no error 1: time-out error) */
#define I8042_STATUS_AUXDATA        BIT(5)  /* */
#define I8042_STATUS_KEYLOCK        BIT(4)  /* Keyboard Inhibited */
#define I8042_STATUS_CMDDAT         BIT(3)  /* Command/data: (0: write data 2: write cmd) */
#define I8042_STATUS_SFLAG          BIT(2)  /* Meant to be cleared on reset and set by firmware */
#define I8042_STATUS_IBF            BIT(1)  /* Input Buffer Full */
#define I8042_STATUS_OBF            BIT(0)  /* Output Buffer Full */

#define I8042_CTRL_XLATE            BIT(6)
#define I8042_CTRL_AUXDIS           BIT(5)
#define I8042_CTRL_KBDDIS           BIT(4)
#define I8042_CTRL_IGNKEYLOCK       BIT(3)
#define I8042_CTRL_AUXINT           BIT(1)
#define I8042_CTRL_KBDINT           BIT(0)

#endif /* _DRIVER_PS2_i8042_H_ */
