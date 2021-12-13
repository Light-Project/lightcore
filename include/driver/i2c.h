/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_i2C_H_
#define _DRIVER_i2C_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <device.h>
#include <resource.h>
#include <devtable.h>

struct i2c_device {
    struct device dev;
    uint16_t addr;

};

struct i2c_driver {
    struct driver driver;
};

struct i2c_transfer {
    uint16_t length;
    uint8_t *buff;
};

struct i2c_host {
    struct i2c_ops *ops;

};

struct i2c_ops {
    state (*transfer)(struct i2c_host *, struct i2c_device *, struct i2c_transfer *);
};

#endif  /* _DRIVER_i2C_H_ */
