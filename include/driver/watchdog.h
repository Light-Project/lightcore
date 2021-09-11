/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_WATCHDOG_H_
#define _DRIVER_WATCHDOG_H_

#include <types.h>
#include <state.h>
#include <list.h>

enum watchdog_status {
    WDT_RUNNING = 0,
};

struct watchdog_device {
    struct list_head list;
    struct watchdog_ops *ops;
    enum watchdog_status status;
    unsigned int timeout_min;
    unsigned int timeout_max;
};

struct watchdog_ops {
    state (*start)(struct watchdog_device *);
    state (*stop)(struct watchdog_device *);
    state (*feed)(struct watchdog_device *);
    state (*timeout_set)(struct watchdog_device *, unsigned int);
};

state watchdog_feed(struct watchdog_device *wdev);

state watchdog_register(struct watchdog_device *);
state watchdog_unregister(struct watchdog_device *);

#endif /* _DRIVER_WATCHDOG_H_ */
