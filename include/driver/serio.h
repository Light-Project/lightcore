/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_SERIO_H_
#define _DRIVER_SERIO_H_

#include <irq.h>
#include <device.h>
#include <devtable.h>

enum serio_type {
    SERIO_TYPE_PS2,
    SERIO_TYPE_PS2_XL,
    SERIO_TYPE_SERIAL,
};

/**
 * struct serio_device - describe a serio device.
 * @dev: kernel infrastructure device architecture.
 * @id: id signature for matching driver.
 */
struct serio_device {
    struct device dev;
    struct serio_device_id id;
};

#define device_to_serio_device(devp) \
    container_of(devp, struct serio_device, dev)

/**
 * struct serio_driver - describe a serio driver.
 * @driver: kernel infrastructure driver architecture.
 * @id_table: list of devices types supported by the driver.
 * @probe: binds this driver to the serio device.
 * @remove: unbinds this driver from the serio device.
 * @shutdown: shutdown serio device with this driver.
 * @reconnect: reconnect serio device with this driver.
 * @interrupt: serio device interrupt for this driver.
 */
struct serio_driver {
    struct driver driver;
    const struct serio_device_id *id_table;

    state (*probe)(struct serio_device *sdev, const void *pdata);
    void (*remove)(struct serio_device *sdev);
    void (*shutdown)(struct serio_device *sdev);

    state (*reconnect)(struct serio_device *sdev);
    irqreturn_t (*interrupt)(struct serio_device *sdev, uint32_t data, const void *pdata);
};

#define driver_to_serio_driver(drv) \
    container_of(drv, struct serio_driver, driver)

/**
 * struct serio_host - describe a serio host.
 * @dev: points to the parent device of the serio host.
 * @lock: resource lock for serio host.
 * @ops: operations method of serio host.
 * @port: the only device connected to the serio host.
 */
struct serio_host {
    struct device *dev;
    spinlock_t lock;
    struct serio_ops *ops;
    struct serio_device port;
};

#define device_to_serio_host(dev) \
    container_of(dev, struct serio_driver, port)

/**
 * struct serio_ops - describe the operations of a serio host.
 * @enable: enable a serio host port after connecting.
 * @disable: disable a serio host port after connecting.
 * @write: write data to the serio host port after connecting.
 */
struct serio_ops {
    state (*start)(struct serio_host *shost);
    void (*stop)(struct serio_host *shost);
    state (*enable)(struct serio_host *shost);
    void (*disable)(struct serio_host *shost);
    state (*write)(struct serio_host *shost, uint8_t cmd);
};

/* serio registration */
extern state serio_driver_register(struct serio_driver *sdev);
extern void serio_driver_unregister(struct serio_driver *sdev);
extern irqreturn_t serio_interrupt(struct serio_host *host, uint32_t data, const void *pdata);
extern state serio_host_register(struct serio_host *host);
extern void serio_host_unregister(struct serio_host *host);

/* convenience logging macros */
#define serio_emerg(sdev, fmt, ...)     dev_emerg(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define serio_alert(sdev, fmt, ...)     dev_alert(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define serio_crit(sdev, fmt, ...)      dev_crit(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define serio_err(sdev, fmt, ...)       dev_err(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define serio_warn(sdev, fmt, ...)      dev_warn(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define serio_notice(sdev, fmt, ...)    dev_notice(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define serio_info(sdev, fmt, ...)      dev_info(&(sdev)->dev, fmt, ##__VA_ARGS__)
#define serio_debug(sdev, fmt, ...)     dev_debug(&(sdev)->dev, fmt, ##__VA_ARGS__)
BUS_DEVRES_GENERIC(serio, struct serio_device, dev)

#endif /* _DRIVER_SERIO_H_ */
