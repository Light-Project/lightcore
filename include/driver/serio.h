/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_SERIO_H_
#define _DRIVER_SERIO_H_

#include <state.h>
#include <irq.h>
#include <spinlock.h>
#include <device.h>
#include <devtable.h>

enum serio_type {
    SERIO_TYPE_PS2,
    SERIO_TYPE_PS2_XL,
    SERIO_TYPE_SERIAL,
};

struct serio_device {
    struct device dev;
    struct serio_host *host;
    struct serio_device_id id;
};

#define device_to_serio_device(devp) \
    container_of(devp, struct serio_device, dev)

struct serio_driver {
    struct driver driver;
    const struct serio_device_id *id_table;

    state (*probe)(struct serio_device *serio, const void *pdata);
    void (*remove)(struct serio_device *serio);
    void (*shutdown)(struct serio_device *serio);
    irqreturn_t (*interrupt)(struct serio_device *, uint32_t data, const void *pdata);
};

#define driver_to_serio_driver(drv) \
    container_of(drv, struct serio_driver, driver)

struct serio_host {
    struct serio_device port;
    spinlock_t lock;
    struct device *dev;
    struct serio_ops *ops;
};

struct serio_ops {
    state (*enable)(struct serio_host *);
    state (*disable)(struct serio_host *);
    state (*write)(struct serio_host *, uint8_t cmd);
};

state serio_driver_register(struct serio_driver *);
void serio_driver_unregister(struct serio_driver *);

irqreturn_t serio_interrupt(struct serio_host *, uint32_t data, const void *pdata);
state serio_host_register(struct serio_host *);
void serio_host_unregister(struct serio_host *);

#endif  /* _DRIVER_SERIO_H_ */
