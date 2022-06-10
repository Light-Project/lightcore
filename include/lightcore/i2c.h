/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_I2C_H_
#define _LIGHTCORE_I2C_H_

#define I2C_FUNC_I2C                        0x00000001
#define I2C_FUNC_10BIT_ADDR                 0x00000002
#define I2C_FUNC_PROTOCOL_MANGLING          0x00000004
#define I2C_FUNC_SMBUS_PEC                  0x00000008
#define I2C_FUNC_NOSTART                    0x00000010
#define I2C_FUNC_SLAVE                      0x00000020
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL      0x00008000
#define I2C_FUNC_SMBUS_QUICK                0x00010000
#define I2C_FUNC_SMBUS_READ_BYTE            0x00020000
#define I2C_FUNC_SMBUS_WRITE_BYTE           0x00040000
#define I2C_FUNC_SMBUS_READ_BYTE_DATA       0x00080000
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA      0x00100000
#define I2C_FUNC_SMBUS_READ_WORD_DATA       0x00200000
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA      0x00400000
#define I2C_FUNC_SMBUS_PROC_CALL            0x00800000
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA      0x01000000
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA     0x02000000
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK       0x04000000
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK      0x08000000
#define I2C_FUNC_SMBUS_HOST_NOTIFY          0x10000000

#define I2C_M_RD                0x0001
#define I2C_M_TEN               0x0010
#define I2C_M_DMA_SAFE          0x0200
#define I2C_M_RECV_LEN          0x0400
#define I2C_M_NO_RD_ACK         0x0800
#define I2C_M_IGNORE_NAK        0x1000
#define I2C_M_REV_DIR_ADDR      0x2000
#define I2C_M_NOSTART           0x4000
#define I2C_M_STOP              0x8000

#define SMBUS_QUICK             0x00
#define SMBUS_BYTE              0x01
#define SMBUS_BYTE_DATA         0x02
#define SMBUS_WORD_DATA         0x03
#define SMBUS_PROC_CALL         0x04
#define SMBUS_BLOCK_DATA        0x05
#define SMBUS_I2C_BLOCK_BROKEN  0x06
#define SMBUS_BLOCK_PROC_CALL   0x07
#define SMBUS_I2C_BLOCK_DATA    0x08

struct i2c_transfer {
    __u16 addr;
    __u16 flags;
    __u16 len;
    __u8 *buf;
};

#define SMBUS_WRITE         0
#define SMBUS_READ          1
#define SMBUS_BLOCK_MAX     32

union smbus_transfer {
    __u8 byte;
    __u16 word;
    __u8 block[SMBUS_BLOCK_MAX + 2];
};

#endif  /* _LIGHTCORE_I2C_H_ */
