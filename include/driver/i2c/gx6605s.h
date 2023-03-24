/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_I2C_GX6605S_H_
#define _DRIVER_I2C_GX6605S_H_

#include <bits.h>

enum gx6605s_i2c_registers {
    GX6605S_I2C_CTRL    = 0x00,
    GX6605S_I2C_DATA    = 0x04,
    GX6605S_I2C_CLK     = 0x08,
};

/********************************************************************************************/
/*      Mnemonic                    value           meaning/usage                           */

#define GX6605S_I2C_CTRL_INTEN      BIT(7)      // interrupt enable
#define GX6605S_I2C_CTRL_EN         BIT(6)      // module enable
#define GX6605S_I2C_CTRL_START      BIT(5)      // start transfer
#define GX6605S_I2C_CTRL_STOP       BIT(4)      // stop transfer
#define GX6605S_I2C_CTRL_DONE       BIT(3)      // operatin finish
#define GX6605S_I2C_CTRL_RACK       BIT(2)      // read ack
#define GX6605S_I2C_CTRL_WACK       BIT(1)      // write ack

/* bug for hardware, if 4 division, it fail often */
#define GX6605S_I2C_CLKDIV(IN, HZ)  (((IN) / (HZ) / 4) - 1)

#endif /* _DRIVER_I2C_GX6605S_H_ */
