/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PS2_H_
#define _DRIVER_PS2_H_

#define PS2_CMD_SEND(size) (((size) & 0xf) << 8)
#define PS2_CMD_RECV(size) (((size) & 0xf) << 12)
#define PS2_CMD(recv, send, cmd) (PS2_CMD_RECV(recv) | PS2_CMD_SEND(send) | (cmd))

#define PS2_GET_CMD(cmd) ((cmd) & 0xff)
#define PS2_GET_SEND(cmd) (((cmd) >> 8) & 0xf)
#define PS2_GET_RECV(cmd) (((cmd) >> 12) & 0xf)

#endif  /* _DRIVER_PS2_H_ */
