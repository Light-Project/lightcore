/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLOCKSOURCE_H_
#define _DRIVER_CLOCKSOURCE_H_

#include <device.h>
#include <driver/clockevent.h>

/**
 * struct clocksource_device - free running counter device
 * @mult: Cycle to nanosecond multiplier
 * @shift: Cycle to nanosecond divisor (power of two)
 * @mask:
 */
struct clocksource_device {
    struct device *device;
    struct list_head list;
    struct clocksource_ops *ops;

    uint64_t mask;
    unsigned int rating;
    unsigned int mult;
    unsigned int shift;
};

/**
 * struct clocksource_ops - free running counter operations
 * @enable: enable the clocksource device
 * @disable: disable the clocksource device
 * @read: returns clocksource cycle value
 */
struct clocksource_ops {
    state (*enable)(struct clocksource_device *);
    state (*disable)(struct clocksource_device *);
    uint64_t (*read)(struct clocksource_device *);
};

#ifndef CONFIG_CLKSRC

static inline void clocksource_config(struct clocksource_device *cdev, uint64_t freq, uint64_t mask)
{

}

static inline state clocksource_config_register(struct clocksource_device *cdev, uint64_t freq, uint64_t mask)
{
    return -ENOERR;
}

static inline state clocksource_register(struct clocksource_device *cdev)
{
    return -ENOERR;
}

static inline void clocksource_unregister(struct clocksource_device *cdev)
{

}

static inline void clocksource_init(void)
{

}

#else

extern void clocksource_config(struct clocksource_device *cdev, uint64_t freq, uint64_t mask);
extern state clocksource_config_register(struct clocksource_device *cdev, uint64_t freq, uint64_t mask);
extern state clocksource_register(struct clocksource_device *cdev);
extern void clocksource_unregister(struct clocksource_device *cdev);
extern void __init clocksource_init(void);

#endif  /* CONFIG_CLKSRC */
#endif  /* _DRIVER_CLOCKSOURCE_H_ */
