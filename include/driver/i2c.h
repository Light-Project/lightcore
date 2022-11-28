/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_I2C_H_
#define _DRIVER_I2C_H_

#include <device.h>
#include <devtable.h>
#include <lightcore/i2c.h>

enum i2c_device_flags {
    __I2C_DEVICE_PEC        = 0, /* Use Packet Error Checking */
    __I2C_DEVICE_TEN        = 1, /* Have a ten bit chip address */
    __I2C_DEVICE_SLAVE      = 2, /* We are the slave  */
    __I2C_DEVICE_NOTIFY     = 3, /* We want to use I2C host notify */
    __I2C_DEVICE_WAKE       = 4,
    __I2C_DEVICE_SCCB       = 5,
};

#define I2C_DEVICE_PEC      BIT(__I2C_DEVICE_PEC)
#define I2C_DEVICE_TEN      BIT(__I2C_DEVICE_TEN)
#define I2C_DEVICE_SLAVE    BIT(__I2C_DEVICE_SLAVE)
#define I2C_DEVICE_NOTIFY   BIT(__I2C_DEVICE_NOTIFY)
#define I2C_DEVICE_WAKE     BIT(__I2C_DEVICE_WAKE)
#define I2C_DEVICE_SCCB     BIT(__I2C_DEVICE_SCCB)

enum i2c_speed_mode {
    I2C_SPEED_STANDARD      = 100000,
    I2C_SPEED_FAST          = 400000,
    I2C_SPEED_FAST_PLUS     = 1000000,
    I2C_SPEED_TURBO         = 1400000,
    I2C_SPEED_HIGH_SPEED    = 3400000,
    I2C_SPEED_ULTRA_FAST    = 5000000,
};

/**
 * struct i2c_device - describe an i2c slave device.
 * @dev: kernel infrastructure device architecture.
 * @name: name used to match the platform_table.
 * @dt_node: device tree node of the device.
 * @host: i2c controller used with the device.
 */
struct i2c_device {
    struct device dev;
    const char *name;
    struct dt_node *dt_node;
    struct i2c_host *host;
    void *pdata;

    uint16_t addr;
    uint16_t flags;
};

#define device_to_i2c_device(ptr) \
    container_of(ptr, struct i2c_device, dev)

static inline void *i2c_get_devdata(const struct i2c_device *idev)
{
    return device_get_pdata(&idev->dev);
}

static inline void i2c_set_devdata(struct i2c_device *idev, void *data)
{
    return device_set_pdata(&idev->dev, data);
}

/**
 * struct i2c_driver - describe an i2c slave driver.
 * @driver: kernel infrastructure driver architecture.
 * @platform_table: list of i2c devices supported by this driver.
 * @dt_table: list of i2c devices supported by this driver.
 * @probe: binds this driver from the i2c device.
 * @remove: unbinds this driver from the i2c device.
 * @shutdown: shutdown i2c device.
 */
struct i2c_driver {
    struct driver driver;
    const struct platform_device_id *platform_table;
    const struct dt_device_id *dt_table;

    state (*probe)(struct i2c_device *idev, const void *pdata);
    void (*remove)(struct i2c_device *idev);
    void (*shutdown)(struct i2c_device *idev);
};

#define driver_to_i2c_driver(ptr) \
    container_of(ptr, struct i2c_driver, driver)

/**
 * struct i2c_host - describe an i2c host.
 * @dev: points to the parent device of the i2c host.
 * @list: linked list for centralized management of i2c host.
 * @ops: operations method of i2c host.
 * @devices: devices attached on this i2c bus.
 */
struct i2c_host {
    struct device *dev;
    struct list_head list;
    struct i2c_ops *ops;
    void *host_data;

    struct list_head devices;
    unsigned int capability;
    unsigned int retries;
    unsigned int timeout;
};

struct i2c_ops {
    state (*i2c_transfer)(struct i2c_host *host, unsigned int num, struct i2c_transfer *trans);
    state (*smbus_transfer)(struct i2c_host *host, uint16_t addr, unsigned short flags, unsigned int dir, uint8_t cmd, unsigned int type, union smbus_transfer *trans);
};

extern spinlock_t i2c_host_lock;
extern struct list_head i2c_host_list;
extern struct bus_type i2c_bus;

extern state i2c_transfer(struct i2c_host *host, unsigned int num, struct i2c_transfer *trans);
extern state i2c_transfer_buffer(struct i2c_device *idev, uint8_t *buff, unsigned int len, uint16_t flags);

static inline state i2c_transfer_read_byte(struct i2c_device *idev, uint8_t *value)
{
    return i2c_transfer_buffer(idev, value, 1, I2C_M_RD);
}

static inline state i2c_transfer_write_byte(struct i2c_device *idev, uint8_t value)
{
    return i2c_transfer_buffer(idev, &value, 1, 0);
}

static inline state i2c_device_transfer(struct i2c_device *idev, unsigned int num, struct i2c_transfer *trans)
{
    return i2c_transfer(idev->host, num, trans);
}

static inline uint8_t i2c_transfer_addr8(const struct i2c_transfer *trans)
{
    return (trans->addr << 1) | !!(trans->flags & I2C_M_RD);
}

/* parallel device matching */
extern void dt_populate_i2c(struct i2c_host *host);

/* i2c registration */
extern const char *i2c_speed_mode_string(unsigned int speed);
extern struct i2c_device *i2c_device_alloc(struct i2c_host *host, const char *name);
extern void i2c_device_free(struct i2c_device *idev);
extern state i2c_device_register(struct i2c_device *idev);
extern void i2c_device_unregister(struct i2c_device *idev);
extern state i2c_driver_register(struct i2c_driver *idrv);
extern void i2c_driver_unregister(struct i2c_driver *idrv);
extern state i2c_host_register(struct i2c_host *host);
extern void i2c_host_unregister(struct i2c_host *host);

/* convenience logging macros */
#define i2c_emerg(idev, fmt, ...)  	dev_emerg(&(idev)->dev, fmt, ##__VA_ARGS__)
#define i2c_alert(idev, fmt, ...)  	dev_alert(&(idev)->dev, fmt, ##__VA_ARGS__)
#define i2c_crit(idev, fmt, ...)   	dev_crit(&(idev)->dev, fmt, ##__VA_ARGS__)
#define i2c_err(idev, fmt, ...)    	dev_err(&(idev)->dev, fmt, ##__VA_ARGS__)
#define i2c_warn(idev, fmt, ...)   	dev_warn(&(idev)->dev, fmt, ##__VA_ARGS__)
#define i2c_notice(idev, fmt, ...) 	dev_notice(&(idev)->dev, fmt, ##__VA_ARGS__)
#define i2c_info(idev, fmt, ...)   	dev_info(&(idev)->dev, fmt, ##__VA_ARGS__)
#define i2c_debug(idev, fmt, ...)  	dev_debug(&(idev)->dev, fmt, ##__VA_ARGS__)
BUS_DEVRES_GENERIC(i2c, struct i2c_device, dev)

#endif  /* _DRIVER_I2C_H_ */
