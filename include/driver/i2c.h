/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_I2C_H_
#define _DRIVER_I2C_H_

#include <device.h>
#include <devtable.h>
#include <lightcore/i2c.h>

/**
 * struct i2c_device -
 * @list:
 * @dev:
 * @addr:
 */
struct i2c_device {
    struct list_head list;
    struct device dev;
    uint16_t addr;
};

/**
 * struct i2c_driver -
 * @driver:
 */
struct i2c_driver {
    struct driver driver;
    const struct dt_device_id *dt_table;

    state (*probe)(struct i2c_device *idev, const void *pdata);
    void (*remove)(struct i2c_device *idev);
    void (*shutdown)(struct i2c_device *idev);
};

/**
 * struct i2c_host - describe a parallel host.
 * @dev: points to the parent device of the i2c host.
 * @list: linked list for centralized management of i2c host.
 * @ops: operations method of i2c host.
 * @devices: devices attached on this i2c bus.
 */
struct i2c_host {
    struct device *dev;
    struct list_head list;
    struct i2c_ops *ops;
    struct list_head devices;
    unsigned int capability;
};

/**
 * struct i2c_ops -
 * @i2c_transfer:
 * @smbus_transfer:
 */
struct i2c_ops {
    state (*i2c_transfer)(struct i2c_host *host, unsigned int num, struct i2c_transfer *trans);
    state (*smbus_transfer)(struct i2c_host *host, uint16_t addr, unsigned short flags, unsigned int dir, uint8_t cmd, unsigned int type, union smbus_transfer *trans);
};

extern spinlock_t i2c_host_lock;
extern struct list_head i2c_host_list;
extern struct bus_type i2c_bus;

extern state i2c_device_register(struct i2c_device *idev);
extern void i2c_device_unregister(struct i2c_device *idev);
extern state i2c_driver_register(struct i2c_driver *idrv);
extern void i2c_driver_unregister(struct i2c_driver *idrv);

extern state i2c_host_register(struct i2c_host *host);
extern void i2c_host_unregister(struct i2c_host *host);

#endif  /* _DRIVER_I2C_H_ */
