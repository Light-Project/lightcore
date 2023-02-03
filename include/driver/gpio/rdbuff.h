/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_GPIO_RDBUFF_H_
#define _DRIVER_GPIO_RDBUFF_H_

#include <bitmap.h>
#include <driver/gpio.h>

struct gpio_rdbuff {
    struct gpio_device gpio;
    unsigned long *pinmap;
    state (*output)(struct gpio_rdbuff *rdbuff, unsigned int port, bool value);
};

#define gpio_to_rdbuff(ptr) \
    container_of(ptr, struct gpio_rdbuff, gpio)

extern state gpio_rdbuff_get(struct gpio_device *gdev, unsigned int port, bool *value);
extern state gpio_rdbuff_set(struct gpio_device *gdev, unsigned int port, bool value);
extern state gpio_rdbuff_init(struct gpio_rdbuff *rdbuff, gfp_t gfp);

#endif /* _DRIVER_GPIO_RDBUFF_H_ */
