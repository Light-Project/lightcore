/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/gpio.h>
#include <export.h>

state gpio_raw_request(struct gpio_device *gdev, unsigned int port)
{
    struct gpio_ops *ops = gdev->ops;

    if (!ops->request)
        return -EOPNOTSUPP;

    return ops->request(gdev, port);
}
EXPORT_SYMBOL(gpio_raw_request);

state gpio_raw_release(struct gpio_device *gdev, unsigned int port)
{
    struct gpio_ops *ops = gdev->ops;

    if (!ops->release)
        return -EOPNOTSUPP;

    return ops->release(gdev, port);
}
EXPORT_SYMBOL(gpio_raw_release);

#define GENERIC_GPIO_RAW_OPS(name, operation, type)                             \
state name(struct gpio_device *gdev, unsigned int index, type value)            \
{                                                                               \
    struct gpio_ops *ops = gdev->ops;                                           \
                                                                                \
    if (!ops->operation)                                                        \
        return -EOPNOTSUPP;                                                     \
                                                                                \
    return ops->operation(gdev, index, value);                                  \
}

GENERIC_GPIO_RAW_OPS(gpio_raw_value_get, value_get, bool *)
GENERIC_GPIO_RAW_OPS(gpio_raw_value_set, value_set, bool)
GENERIC_GPIO_RAW_OPS(gpio_raw_direction_get, direction_get, enum gpio_direction *)
GENERIC_GPIO_RAW_OPS(gpio_raw_direction_set, direction_set, enum gpio_direction)
GENERIC_GPIO_RAW_OPS(gpio_raw_pull_get, pull_get, enum gpio_pull *)
GENERIC_GPIO_RAW_OPS(gpio_raw_pull_set, pull_set, enum gpio_pull)
GENERIC_GPIO_RAW_OPS(gpio_raw_drive_get, drive_get, enum gpio_drive *)
GENERIC_GPIO_RAW_OPS(gpio_raw_drive_set, drive_set, enum gpio_drive)
GENERIC_GPIO_RAW_OPS(gpio_raw_speed_get, speed_get, enum gpio_speed *)
GENERIC_GPIO_RAW_OPS(gpio_raw_speed_set, speed_set, enum gpio_speed)

EXPORT_SYMBOL(gpio_raw_value_get);
EXPORT_SYMBOL(gpio_raw_value_set);
EXPORT_SYMBOL(gpio_raw_direction_get);
EXPORT_SYMBOL(gpio_raw_direction_set);
EXPORT_SYMBOL(gpio_raw_pull_get);
EXPORT_SYMBOL(gpio_raw_pull_set);
EXPORT_SYMBOL(gpio_raw_drive_get);
EXPORT_SYMBOL(gpio_raw_drive_set);
EXPORT_SYMBOL(gpio_raw_speed_get);
EXPORT_SYMBOL(gpio_raw_speed_set);

#define GENERIC_GPIOC_RAW_OPS(name, operation, type)    \
state name(struct gpio_channel *channel, type value)    \
{                                                       \
    struct gpio_device *gdev = channel->gdev;           \
    unsigned int index = channel->index;                \
    return operation(gdev, index, value);               \
}

GENERIC_GPIOC_RAW_OPS(gpioc_raw_value_get, gpio_raw_value_get, bool *)
GENERIC_GPIOC_RAW_OPS(gpioc_raw_value_set, gpio_raw_value_set, bool)
GENERIC_GPIOC_RAW_OPS(gpioc_raw_direction_get, gpio_raw_direction_get, enum gpio_direction *)
GENERIC_GPIOC_RAW_OPS(gpioc_raw_direction_set, gpio_raw_direction_set, enum gpio_direction)
GENERIC_GPIOC_RAW_OPS(gpioc_raw_pull_get, gpio_raw_pull_get, enum gpio_pull *)
GENERIC_GPIOC_RAW_OPS(gpioc_raw_pull_set, gpio_raw_pull_set, enum gpio_pull)
GENERIC_GPIOC_RAW_OPS(gpioc_raw_drive_get, gpio_raw_drive_get, enum gpio_drive *)
GENERIC_GPIOC_RAW_OPS(gpioc_raw_drive_set, gpio_raw_drive_set, enum gpio_drive)
GENERIC_GPIOC_RAW_OPS(gpioc_raw_speed_get, gpio_raw_speed_get, enum gpio_speed *)
GENERIC_GPIOC_RAW_OPS(gpioc_raw_speed_set, gpio_raw_speed_set, enum gpio_speed)

EXPORT_SYMBOL(gpioc_raw_value_get);
EXPORT_SYMBOL(gpioc_raw_value_set);
EXPORT_SYMBOL(gpioc_raw_direction_get);
EXPORT_SYMBOL(gpioc_raw_direction_set);
EXPORT_SYMBOL(gpioc_raw_pull_get);
EXPORT_SYMBOL(gpioc_raw_pull_set);
EXPORT_SYMBOL(gpioc_raw_drive_get);
EXPORT_SYMBOL(gpioc_raw_drive_set);
EXPORT_SYMBOL(gpioc_raw_speed_get);
EXPORT_SYMBOL(gpioc_raw_speed_set);
