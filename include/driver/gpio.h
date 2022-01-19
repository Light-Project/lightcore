/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_GPIO_H_
#define _DRIVER_GPIO_H_

#include <list.h>
#include <device.h>

enum gpio_pull {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_LOW  = 1,
    GPIO_PULL_HIGH = 2,
};

enum gpio_drv {
    GPIO_DRIVER_WEAKEST     = 0,
    GPIO_DRIVER_WEAKER      = 1,
    GPIO_DRIVER_WEAK        = 2,
    GPIO_DRIVER_STRONG      = 3,
    GPIO_DRIVER_STRONGER    = 4,
    GPIO_DRIVER_STRONGEST   = 5,
};

enum gpio_direction {
    GPIO_DIRECTION_INPUT    = 0,
    GPIO_DIRECTION_OUTPUT   = 1,
};

struct gpio_device {
    struct list_head list;
    struct deivce *parent;
    struct gpio_ops *ops;
};

struct gpio_ops {
    bool (*value_get)(struct gpio_device *, unsigned int port);
    void (*value_set)(struct gpio_device *, unsigned int port, bool value);
    enum gpio_direction (*direction_get)(struct gpio_device *, unsigned int port);
    void (*direction_set)(struct gpio_device *, unsigned int port, enum gpio_direction);
    enum gpio_drv (*drv_get)(struct gpio_device *, unsigned int port);
    void (*drv_set)(struct gpio_device *, unsigned int port, enum gpio_drv);
    enum gpio_pull (*pull_get)(struct gpio_device *, unsigned int port);
    void (*pull_set)(struct gpio_device *, unsigned int port, enum gpio_pull);
};

state gpio_register(struct gpio_device *);
state gpio_unregister(struct gpio_device *);

#endif /* _FRAMEWORK_GPIO_GPIO_H_ */
