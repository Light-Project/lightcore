/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/gpio/rdbuff.h>
#include <export.h>

state gpio_rdbuff_get(struct gpio_device *gdev, unsigned int port, bool *value)
{
    struct gpio_rdbuff *rdbuff = gpio_to_rdbuff(gdev);
    *value = bit_atomic_test(rdbuff->pinmap, port);
    return -ENOERR;
}
EXPORT_SYMBOL(gpio_rdbuff_get);

state gpio_rdbuff_set(struct gpio_device *gdev, unsigned int port, bool value)
{
    struct gpio_rdbuff *rdbuff = gpio_to_rdbuff(gdev);
    arch_bit_atomic_change(rdbuff->pinmap, port, value);
    rdbuff->output(rdbuff, port, value);
    return -ENOERR;
}
EXPORT_SYMBOL(gpio_rdbuff_set);

state gpio_rdbuff_init(struct gpio_rdbuff *rdbuff, gfp_t gfp)
{
    struct gpio_device *gdev = &rdbuff->gpio;

    rdbuff->pinmap = bitmap_zalloc(gdev->gpio_num, gfp);
    if (unlikely(!rdbuff->pinmap))
        return -ENOMEM;

    return -ENOERR;
}
EXPORT_SYMBOL(gpio_rdbuff_init);
