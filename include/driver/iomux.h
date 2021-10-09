/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_IOMUX_H_
#define _DRIVER_IOMUX_H_

#include <types.h>
#include <state.h>
#include <list.h>

enum iomux_direction {
    IOMUX_DIR_INPUT         = 0,
    IOMUX_DIR_OUTPUT        = 1,
};

enum iomux_pull {
    IOMUX_PUL_NONE          = 0,
    IOMUX_PUL_LOW           = 1,
    IOMUX_PUL_HIGH          = 2,
};

enum iomux_drv {
    IOMUX_DRV_WEAKEST       = 0,
    IOMUX_DRV_WEAKER        = 1,
    IOMUX_DRV_WEAK          = 2,
    IOMUX_DRV_STRONG        = 3,
    IOMUX_DRV_STRONGER      = 4,
    IOMUX_DRV_STRONGEST     = 5,
};

struct iomux_desc {
    const char *name;
    unsigned int index;
};

struct iomux_device {
    struct list_head list;
    struct iomux_ops *ops;
};

struct iomux_ops {
    state (*request)(struct ioctl_device *, unsigned int index);
    state (*release)(struct ioctl_device *, unsigned int index);

    unsigned int (*get_function)(struct ioctl_device *, unsigned int index);
    state (*set_function)(struct ioctl_device *, unsigned int index);

    enum iomux_direction (*direction_get)(struct gpio_device *, unsigned int port);
    void (*direction_set)(struct gpio_device *, unsigned int port, enum iomux_direction);

    enum iomux_pull (*pull_get)(struct gpio_device *, unsigned int port);
    void (*pull_set)(struct gpio_device *, unsigned int port, enum gpio_pull);

    enum iomux_drv (*drv_get)(struct gpio_device *, unsigned int port);
    void (*drv_set)(struct gpio_device *, unsigned int port, enum gpio_drv);
};

#endif  /* _DRIVER_IOMUX_H_ */
