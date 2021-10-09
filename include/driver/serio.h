/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_SERIO_H_
#define _DRIVER_SERIO_H_

#include <state.h>
#include <irq.h>
#include <device.h>
#include <spinlock.h>

struct serio_device {
    struct device dev;
    struct serio_host *host;
};

#define device_to_serio_device(devp)	\
    container_of(devp, struct serio_device, dev)

struct serio_driver {
    struct driver driver;

    state (*probe)(struct serio_device *serio, void *pdata);
    void (*remove)(struct serio_device *serio);
    void (*shutdown)(struct serio_device *serio);
    irqreturn_t (*interrupt)(struct serio_device *, uint32_t data, void *pdata);
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

irqreturn_t serio_interrupt(struct serio_host *, uint32_t data, void *pdata);
state serio_host_register(struct serio_host *);
void serio_host_unregister(struct serio_host *);

#endif  /* _DRIVER_SERIO_H_ */
