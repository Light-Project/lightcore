/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_INPUT_ATKBD_H_
#define _DRIVER_INPUT_ATKBD_H_

#include <driver/serio/ps2.h>

enum atkbd_command {
    /* Command Byte */
    ATKBD_CMD_OK_GETID      = PS2_CMD(2, 0, 0xe8),
    ATKBD_CMD_EX_ENABLE     = PS2_CMD(0, 1, 0xea),
    ATKBD_CMD_EX_SETLEDS    = PS2_CMD(0, 2, 0xeb),
    ATKBD_CMD_SETLEDS       = PS2_CMD(0, 1, 0xed),
    ATKBD_CMD_SSCANSET      = PS2_CMD(0, 1, 0xf0),
    ATKBD_CMD_GSCANSET      = PS2_CMD(1, 1, 0xf0),
    ATKBD_CMD_GETID         = PS2_CMD(2, 0, 0xf2),
    ATKBD_CMD_SETREP        = PS2_CMD(0, 1, 0xf3),
    ATKBD_CMD_ENABLE        = PS2_CMD(0, 0, 0xf4),
    ATKBD_CMD_RESET_DIS     = PS2_CMD(0, 0, 0xf5),
    ATKBD_CMD_RESET_DEF     = PS2_CMD(0, 0, 0xf6),
    ATKBD_CMD_SETALL_MB     = PS2_CMD(0, 0, 0xf8),
    ATKBD_CMD_SETALL_MBR    = PS2_CMD(0, 0, 0xfa),
    ATKBD_CMD_RESEND        = PS2_CMD(0, 0, 0xfe),
    ATKBD_CMD_RESET_BAT     = PS2_CMD(2, 0, 0xff),

    /* Response Byte */
    ATKBD_RET_BAT           = 0xaa,
    ATKBD_RET_EMUL0         = 0xe0,
    ATKBD_RET_EMUL1         = 0xe1,
    ATKBD_RET_RELEASE       = 0xf0,
    ATKBD_RET_HANJA         = 0xf1,
    ATKBD_RET_HANGEUL       = 0xf2,
    ATKBD_RET_ACK           = 0xfa,
    ATKBD_RET_NAK           = 0xfe,
    ATKBD_RET_ERR           = 0xff,
};

#endif /* _DRIVER_INPUT_ATKBD_H_ */
