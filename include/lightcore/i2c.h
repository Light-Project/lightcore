/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_I2C_H_
#define _LIGHTCORE_I2C_H_

enum i2c_func_flags {
    __I2C_FUNC_I2C = 0,
    __I2C_FUNC_LSB,
    __I2C_FUNC_10BIT_ADDR,
    __I2C_FUNC_PROTOCOL_MANGLING,
    __I2C_FUNC_SMBUS_PEC,
    __I2C_FUNC_NOSTART,
    __I2C_FUNC_SLAVE,
    __I2C_FUNC_SMBUS_BLOCK_PROC_CALL,
    __I2C_FUNC_SMBUS_QUICK,
    __I2C_FUNC_SMBUS_READ_BYTE,
    __I2C_FUNC_SMBUS_WRITE_BYTE,
    __I2C_FUNC_SMBUS_READ_BYTE_DATA,
    __I2C_FUNC_SMBUS_WRITE_BYTE_DATA,
    __I2C_FUNC_SMBUS_READ_WORD_DATA,
    __I2C_FUNC_SMBUS_WRITE_WORD_DATA,
    __I2C_FUNC_SMBUS_PROC_CALL,
    __I2C_FUNC_SMBUS_READ_BLOCK_DATA,
    __I2C_FUNC_SMBUS_WRITE_BLOCK_DATA,
    __I2C_FUNC_SMBUS_READ_I2C_BLOCK,
    __I2C_FUNC_SMBUS_WRITE_I2C_BLOCK,
    __I2C_FUNC_SMBUS_HOST_NOTIFY,
};

#define I2C_FUNC_I2C                        (1UL << __I2C_FUNC_I2C)
#define I2C_FUNC_LSB                        (1UL << __I2C_FUNC_LSB)
#define I2C_FUNC_10BIT_ADDR                 (1UL << __I2C_FUNC_10BIT_ADDR)
#define I2C_FUNC_PROTOCOL_MANGLING          (1UL << __I2C_FUNC_PROTOCOL_MANGLING)
#define I2C_FUNC_SMBUS_PEC                  (1UL << __I2C_FUNC_SMBUS_PEC)
#define I2C_FUNC_NOSTART                    (1UL << __I2C_FUNC_NOSTART)
#define I2C_FUNC_SLAVE                      (1UL << __I2C_FUNC_SLAVE)
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL      (1UL << __I2C_FUNC_SMBUS_BLOCK_PROC_CALL)
#define I2C_FUNC_SMBUS_QUICK                (1UL << __I2C_FUNC_SMBUS_QUICK)
#define I2C_FUNC_SMBUS_READ_BYTE            (1UL << __I2C_FUNC_SMBUS_READ_BYTE)
#define I2C_FUNC_SMBUS_WRITE_BYTE           (1UL << __I2C_FUNC_SMBUS_WRITE_BYTE)
#define I2C_FUNC_SMBUS_READ_BYTE_DATA       (1UL << __I2C_FUNC_SMBUS_READ_BYTE_DATA)
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA      (1UL << __I2C_FUNC_SMBUS_WRITE_BYTE_DATA)
#define I2C_FUNC_SMBUS_READ_WORD_DATA       (1UL << __I2C_FUNC_SMBUS_READ_WORD_DATA)
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA      (1UL << __I2C_FUNC_SMBUS_WRITE_WORD_DATA)
#define I2C_FUNC_SMBUS_PROC_CALL            (1UL << __I2C_FUNC_SMBUS_PROC_CALL)
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA      (1UL << __I2C_FUNC_SMBUS_READ_BLOCK_DATA)
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA     (1UL << __I2C_FUNC_SMBUS_WRITE_BLOCK_DATA)
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK       (1UL << __I2C_FUNC_SMBUS_READ_I2C_BLOCK)
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK      (1UL << __I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)
#define I2C_FUNC_SMBUS_HOST_NOTIFY          (1UL << __I2C_FUNC_SMBUS_HOST_NOTIFY)

enum i2c_master_flags {
    __I2C_M_RD = 0,
    __I2C_M_LSB,
    __I2C_M_TEN,
    __I2C_M_DMA_SAFE,
    __I2C_M_RECV_LEN,
    __I2C_M_NO_RD_ACK,
    __I2C_M_IGNORE_NAK,
    __I2C_M_REV_DIR_ADDR,
    __I2C_M_NOSTART,
    __I2C_M_STOP,
};

#define I2C_M_RD                    (1UL << __I2C_M_RD)
#define I2C_M_LSB                   (1UL << __I2C_M_LSB)
#define I2C_M_TEN                   (1UL << __I2C_M_TEN)
#define I2C_M_DMA_SAFE              (1UL << __I2C_M_DMA_SAFE)
#define I2C_M_RECV_LEN              (1UL << __I2C_M_RECV_LEN)
#define I2C_M_NO_RD_ACK             (1UL << __I2C_M_NO_RD_ACK)
#define I2C_M_IGNORE_NAK            (1UL << __I2C_M_IGNORE_NAK)
#define I2C_M_REV_DIR_ADDR          (1UL << __I2C_M_REV_DIR_ADDR)
#define I2C_M_NOSTART               (1UL << __I2C_M_NOSTART)
#define I2C_M_STOP                  (1UL << __I2C_M_STOP)

enum smbus_flags {
    __SMBUS_QUICK = 0,
    __SMBUS_BYTE,
    __SMBUS_BYTE_DATA,
    __SMBUS_WORD_DATA,
    __SMBUS_PROC_CALL,
    __SMBUS_BLOCK_DATA,
    __SMBUS_I2C_BLOCK_BROKEN,
    __SMBUS_BLOCK_PROC_CALL,
    __SMBUS_I2C_BLOCK_DATA,
};

#define SMBUS_QUICK                 (1UL << __SMBUS_QUICK)
#define SMBUS_BYTE                  (1UL << __SMBUS_BYTE)
#define SMBUS_BYTE_DATA             (1UL << __SMBUS_BYTE_DATA)
#define SMBUS_WORD_DATA             (1UL << __SMBUS_WORD_DATA)
#define SMBUS_PROC_CALL             (1UL << __SMBUS_PROC_CALL)
#define SMBUS_BLOCK_DATA            (1UL << __SMBUS_BLOCK_DATA)
#define SMBUS_I2C_BLOCK_BROKEN      (1UL << __SMBUS_I2C_BLOCK_BROKEN)
#define SMBUS_BLOCK_PROC_CALL       (1UL << __SMBUS_BLOCK_PROC_CALL)
#define SMBUS_I2C_BLOCK_DATA        (1UL << __SMBUS_I2C_BLOCK_DATA)

struct i2c_transfer {
    __u16 addr;
    __u16 flags;
    __u16 len;
    __u8 *buf;
};

#define SMBUS_WRITE 0
#define SMBUS_READ 1
#define SMBUS_BLOCK_MAX 32

union smbus_transfer {
    __u8 byte;
    __u16 word;
    __u8 block[SMBUS_BLOCK_MAX + 2];
};

#endif  /* _LIGHTCORE_I2C_H_ */
