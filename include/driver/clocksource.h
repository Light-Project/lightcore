/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLOCKSOURCE_H_
#define _DRIVER_CLOCKSOURCE_H_

#include <driver/clockevent.h>

/**
 * clocksource_flags - clocksource flags
 * @valid_hres: counter can work at oneshot mode
 * @unstable: counter is unstable
 * @verify_percpu:
 * @suspend_nonstop: counter will not stop in suspend state
 */
enum clocksource_flags {
    CLKSRC_CONTINUOUS       = BIT(0),
    CLKSRC_VALID_HRES       = BIT(1),
    CLKSRC_UNSTABLE         = BIT(2),
    CLKSRC_MUST_VERIFY      = BIT(3),
    CLKSRC_VERIFY_PERCPU    = BIT(4),
    CLKSRC_SUSPEND_NONSTOP  = BIT(5),
};

/**
 * struct clocksource_device - free running counter device
 * @mult: Cycle to nanosecond multiplier
 * @shift: Cycle to nanosecond divisor (power of two)
 * @mask:
 */
struct clocksource_device {
    struct list_head list;
    struct device *device;
    struct clocksource_ops *ops;

    uint64_t mask;
    unsigned int mult;
    unsigned int shift;
    unsigned int rating;
    enum clocksource_flags flags;
};

/**
 * struct clocksource_ops - free running counter operations
 * @enable: enable the clocksource device
 * @disable: disable the clocksource device
 * @read: returns clocksource cycle value
 */
struct clocksource_ops {
    uint64_t (*read)(struct clocksource_device *);
    state (*enable)(struct clocksource_device *);
    state (*disable)(struct clocksource_device *);
    state (*suspend)(struct clocksource_device *);
    state (*resume)(struct clocksource_device *);
};

extern void clocksource_config(struct clocksource_device *cdev, uint64_t freq, uint64_t mask);
extern state clocksource_config_register(struct clocksource_device *cdev, uint64_t freq, uint64_t mask);
extern state clocksource_register(struct clocksource_device *cdev);
extern void clocksource_unregister(struct clocksource_device *cdev);
extern void __init clocksource_init(void);

#endif  /* _DRIVER_CLOCKSOURCE_H_ */
