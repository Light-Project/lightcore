/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PS2_H_
#define _DRIVER_PS2_H_

#define PS2_CMD_SEND(size) ((size) << 8)
#define PS2_CMD_RECV(size) ((size) << 12)
#define PS2_CMD(recv, send, cmd) (PS2_CMD_RECV(recv) | PS2_CMD_SEND(send) | cmd)

#define ps2_cmd_send(cmd) (((cmd) >> 8) & 0xf)
#define ps2_cmd_recv(cmd) (((cmd) >> 12) & 0xf)

#endif  /* _DRIVER_PS2_H_ */
