/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_WATCHDOG_H_
#define _DRIVER_WATCHDOG_H_

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
    state (*start)(struct watchdog_device *wdev);
    state (*stop)(struct watchdog_device *wdev);
    state (*feed)(struct watchdog_device *wdev);
    state (*timeleft_get)(struct watchdog_device *wdev, unsigned int *secs);
    state (*timeout_get)(struct watchdog_device *wdev, unsigned int *secs);
    state (*timeout_set)(struct watchdog_device *wdev, unsigned int secs);
};

extern struct list_head watchdog_list;
extern spinlock_t watchdog_lock;

extern state watchdog_start(struct watchdog_device *wdev);
extern state watchdog_stop(struct watchdog_device *wdev);
extern state watchdog_feed(struct watchdog_device *wdev);
extern state watchdog_timeleft_get(struct watchdog_device *wdev, unsigned int *secs);
extern state watchdog_timeout_get(struct watchdog_device *wdev, unsigned int *secs);
extern state watchdog_timeout_set(struct watchdog_device *wdev, unsigned int secs);

extern state watchdog_register(struct watchdog_device *wdev);
extern void watchdog_unregister(struct watchdog_device *wdev);

#endif /* _DRIVER_WATCHDOG_H_ */
