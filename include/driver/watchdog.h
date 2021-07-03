#ifndef _DRIVER_WATCHDOG_H_
#define _DRIVER_WATCHDOG_H_

#include <types.h>
#include <state.h>

struct watchdog_device;

struct watchdog_ops {
    state (*start)(struct watchdog_device *);
    state (*stop)(struct watchdog_device *);
    state (*feed)(struct watchdog_device *);
    state (*set_timeout)(struct watchdog_device *, uint);
};

enum watchdog_status {
    WDT_RUNNING = 0, 

};

struct watchdog_device {
    list_t list;
    void *base;

    struct watchdog_ops *ops;

    uint min_timeout;
    uint max_timeout;

    enum watchdog_status status;
    void *dev_data;
};

state watchdog_register(struct watchdog_device *);


state watchdog_feed(struct watchdog_device *wdev);

#endif /* _DRIVER_WATCHDOG_H_ */
