/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_WATCHDOG_H_
#define _DRIVER_WATCHDOG_H_

#include <bits.h>
#include <list.h>
#include <spinlock.h>

struct watchdog_device {
    struct list_head list;
    struct watchdog_ops *ops;
    struct device *dev;

    unsigned int timeout_min;
    unsigned int timeout_max;
    uint8_t running:1;
};

struct watchdog_ops {
    state (*start)(struct watchdog_device *);
    state (*stop)(struct watchdog_device *);
    state (*feed)(struct watchdog_device *);
    state (*timeleft_get)(struct watchdog_device *, unsigned int *secs);
    state (*timeout_get)(struct watchdog_device *, unsigned int *secs);
    state (*timeout_set)(struct watchdog_device *, unsigned int secs);
};

state watchdog_start(struct watchdog_device *);
state watchdog_stop(struct watchdog_device *);
state watchdog_feed(struct watchdog_device *);

state watchdog_register(struct watchdog_device *);
void watchdog_unregister(struct watchdog_device *);

#endif /* _DRIVER_WATCHDOG_H_ */
