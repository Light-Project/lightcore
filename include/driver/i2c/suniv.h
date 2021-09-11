/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_I2C_SUNIV_H_
#define _DRIVER_I2C_SUNIV_H_

enum suniv_i2c_registers {
    SUNIV_I2C_ADDR      = 0x00,
    SUNIV_I2C_XADDR     = 0x04,
    SUNIV_I2C_DATA      = 0x08,
    SUNIV_I2C_CNTR      = 0x0c,
    SUNIV_I2C_STAT      = 0x10,
    SUNIV_I2C_CCR       = 0x14,
    SUNIV_I2C_SRST      = 0x18,
    SUNIV_I2C_EFR       = 0x1c,
    SUNIV_I2C_LCR       = 0x20,
};

#endif  /* _DRIVER_I2C_SUNIV_H_ */
