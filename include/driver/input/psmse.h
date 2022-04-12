/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_INPUT_PSMSE_H_
#define _DRIVER_INPUT_PSMSE_H_

#include <driver/serio/ps2.h>

enum psmse_command {
    /* Command Byte */
    PSMSE_CMD_SETSCALE11    = PS2_CMD(0, 0, 0xe6),
    PSMSE_CMD_SETSCALE21    = PS2_CMD(0, 0, 0xe7),
    PSMSE_CMD_SETRES        = PS2_CMD(0, 1, 0xe8),
    PSMSE_CMD_GETINFO       = PS2_CMD(3, 0, 0xe9),
    PSMSE_CMD_SETSTREAM     = PS2_CMD(0, 0, 0xea),
    PSMSE_CMD_POLL          = PS2_CMD(0, 0, 0xeb),
    PSMSE_CMD_RESET_WRAP    = PS2_CMD(0, 0, 0xec),
    PSMSE_CMD_SETPOLL       = PS2_CMD(0, 0, 0xf0),
    PSMSE_CMD_GETID         = PS2_CMD(2, 0, 0xf2),
    PSMSE_CMD_SETRATE       = PS2_CMD(0, 1, 0xf3),
    PSMSE_CMD_ENABLE        = PS2_CMD(0, 0, 0xf4),
    PSMSE_CMD_DISABLE       = PS2_CMD(0, 0, 0xf5),
    PSMSE_CMD_RESET_DIS     = PS2_CMD(0, 0, 0xf6),
    PSMSE_CMD_RESET_BAT     = PS2_CMD(2, 0, 0xff),

    /* Response Byte */
    PSMSE_RET_BAT           = 0xaa,
    PSMSE_RET_ID            = 0x00,
    PSMSE_RET_ACK           = 0xfa,
    PSMSE_RET_NAK           = 0xfe,
};

#endif  /* _DRIVER_INPUT_PSMSE_H_ */