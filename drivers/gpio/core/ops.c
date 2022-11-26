/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/gpio.h>
#include <mhelper.h>
#include <export.h>

#define GENERIC_GPIO_RAW_NOPS(operation, argnr, ...)                            \
state gpio_raw_##operation                                                      \
(struct gpio_device *gdev, MMAP_DECLN(argnr, MARGFN_DECL, __VA_ARGS__))         \
{                                                                               \
    struct gpio_ops *ops = gdev->ops;                                           \
                                                                                \
    if (!ops->operation)                                                        \
        return -EOPNOTSUPP;                                                     \
                                                                                \
    return ops->operation(gdev, MMAP_DECLN(argnr, MARGFN_ARGS, __VA_ARGS__));   \
}                                                                               \
EXPORT_SYMBOL(gpio_raw_##operation)

#define GENERIC_GPIO_RAW_OPS(operation, ...) \
    GENERIC_GPIO_RAW_NOPS(operation, COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

GENERIC_GPIO_RAW_OPS(request, unsigned int);
GENERIC_GPIO_RAW_OPS(release, unsigned int);
GENERIC_GPIO_RAW_OPS(dt_xlate, const struct dt_phandle_args *, uint32_t *, uint32_t *);

GENERIC_GPIO_RAW_OPS(value_get, unsigned int, bool *);
GENERIC_GPIO_RAW_OPS(value_set, unsigned int, bool);
GENERIC_GPIO_RAW_OPS(direction_get, unsigned int, enum gpio_direction *);
GENERIC_GPIO_RAW_OPS(direction_set, unsigned int, enum gpio_direction);
GENERIC_GPIO_RAW_OPS(pull_get, unsigned int, enum gpio_pull *);
GENERIC_GPIO_RAW_OPS(pull_set, unsigned int, enum gpio_pull);
GENERIC_GPIO_RAW_OPS(drive_get, unsigned int, enum gpio_drive *);
GENERIC_GPIO_RAW_OPS(drive_set, unsigned int, enum gpio_drive);
GENERIC_GPIO_RAW_OPS(speed_get, unsigned int, enum gpio_speed *);
GENERIC_GPIO_RAW_OPS(speed_set, unsigned int, enum gpio_speed);

#define GENERIC_GPIOC_RAW_OPS(operation, argnr, ...)                            \
state gpioc_raw_##operation                                                     \
(struct gpio_channel *channel, MMAP_DECLN(argnr, MARGFN_DECL, __VA_ARGS__))     \
{                                                                               \
    struct gpio_device *gdev = channel->gdev;                                   \
    unsigned int index = channel->index;                                        \
    return gpio_raw_##operation(gdev, index,                                    \
                MMAP_DECLN(argnr, MARGFN_ARGS, __VA_ARGS__));                   \
}                                                                               \
EXPORT_SYMBOL(gpioc_raw_##operation)

GENERIC_GPIOC_RAW_OPS(value_get, 1, bool *);
GENERIC_GPIOC_RAW_OPS(value_set, 1, bool);
GENERIC_GPIOC_RAW_OPS(direction_get, 1, enum gpio_direction *);
GENERIC_GPIOC_RAW_OPS(direction_set, 1, enum gpio_direction);
GENERIC_GPIOC_RAW_OPS(pull_get, 1, enum gpio_pull *);
GENERIC_GPIOC_RAW_OPS(pull_set, 1, enum gpio_pull);
GENERIC_GPIOC_RAW_OPS(drive_get, 1, enum gpio_drive *);
GENERIC_GPIOC_RAW_OPS(drive_set, 1, enum gpio_drive);
GENERIC_GPIOC_RAW_OPS(speed_get, 1, enum gpio_speed *);
GENERIC_GPIOC_RAW_OPS(speed_set, 1, enum gpio_speed);

static state gpioc_soft_drain_set(struct gpio_channel *channel, bool value)
{
    state retval;

    if (value)
        retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_INPUT);
    else {
        retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_PUSH);
        if (!retval)
            retval = gpioc_raw_value_set(channel, false);
    }

    if (retval)
        gpioc_warn(channel, "failed set soft drain value (%d) err (%d)\n",
                   value, retval);

    return retval;
}

static state gpioc_soft_source_set(struct gpio_channel *channel, bool value)
{
    state retval;

    if (!value)
        retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_INPUT);
    else {
        retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_PUSH);
        if (!retval)
            retval = gpioc_raw_value_set(channel, true);
    }

    if (retval)
        gpioc_warn(channel, "failed set soft source value (%d) err (%d)\n",
                   value, retval);

    return retval;
}

state gpioc_value_set(struct gpio_channel *channel, bool value)
{
    state retval;

    if (gpioc_test_active_low(channel))
        value = !value;
    if (gpioc_test_soft_drain(channel))
        return gpioc_soft_drain_set(channel, value);
    else if (gpioc_test_soft_source(channel))
        return gpioc_soft_source_set(channel, value);
    else {
        retval = gpioc_raw_value_set(channel, value);
        if (retval)
            gpioc_warn(channel, "failed set value (%d) err (%d)\n",
                       value, retval);
        return retval;
    }
}

state gpioc_value_get(struct gpio_channel *channel, bool *value)
{
    state retval;

    retval = gpioc_raw_value_get(channel, value);
    if (retval) {
        gpioc_warn(channel, "failed get value err (%d)\n",
                   retval);
        return retval;
    }

    if (gpioc_test_active_low(channel))
        *value = !*value;

    return -ENOERR;
}

state gpioc_lookup_configure(struct gpio_channel *channel, enum gpio_lookup_flags flags)
{
    state retval;

    if (flags & GPIOL_FLOAT_INPUT)
        retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_INPUT);
    else if (!(flags & (GPIOL_OPEN_DRAIN | GPIOL_OPEN_SOURCE)))
        retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_PUSH);
    else {
        if (flags & GPIOL_OPEN_DRAIN)
            retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_DRAIN);
        else
            retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_SOURCE);
        if (retval) {
            if (flags & GPIOL_OPEN_SOURCE)
                gpioc_set_soft_drain(channel);
            else
                gpioc_set_soft_source(channel);
            retval = -ENOERR;
        }
    }

    if (retval)
        return retval;

    if (flags & GPIOL_ACTIVE_LOW)
        gpioc_set_active_low(channel);

    if ((flags & GPIOL_PULL_UP) && (flags & GPIOL_PULL_DOWN)) {
        gpioc_err(channel, "both pull-up and pull-down enabled\n");
        return -EINVAL;
    }

    if (flags & GPIOL_PULL_UP)
        retval = gpioc_raw_pull_set(channel, GPIO_PULL_UP);
    else if (flags & GPIOL_PULL_DOWN)
        retval = gpioc_raw_pull_set(channel, GPIO_PULL_DOWN);
    else {
        retval = gpioc_raw_pull_set(channel, GPIO_PULL_FLOAT);
        if (retval == -EOPNOTSUPP)
            retval = -ENOERR;
    }

    return retval;
}
EXPORT_SYMBOL(gpioc_lookup_configure);

state gpioc_request_configure(struct gpio_channel *channel, enum gpio_request_flags flags)
{
    enum gpio_direction dir;
    state retval;

    if (!(flags & GPIOR_FLAG_DIR_SET))
        return -ENOERR;

    retval = gpioc_raw_direction_get(channel, &dir);
    if (retval)
        return retval;

    if ((flags & GPIOR_FLAG_DIR_IN) && dir != GPIO_DIRECTION_INPUT)
        return gpioc_raw_direction_set(channel, GPIO_DIRECTION_INPUT);

    if (flags & GPIOR_FLAG_OPEN_DRAIN) {
        if (dir != GPIO_DIRECTION_DRAIN && !gpioc_test_soft_drain(channel))
            retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_DRAIN);
    } else if (flags & GPIOR_FLAG_OPEN_SOURCE) {
        if (dir != GPIO_DIRECTION_SOURCE && !gpioc_test_soft_source(channel))
            retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_SOURCE);
    } else if (dir != GPIO_DIRECTION_PUSH)
        retval = gpioc_raw_direction_set(channel, GPIO_DIRECTION_PUSH);

    if (retval)
        return retval;

    return gpioc_value_set(channel, !!(flags & GPIOR_FLAG_DIR_VAL));
}
EXPORT_SYMBOL(gpioc_request_configure);

state gpioc_configure(struct gpio_channel *channel, enum gpio_lookup_flags lflags,
                      enum gpio_request_flags rflags)
{
    state retval;

    retval = gpioc_lookup_configure(channel, lflags);
    if (retval)
        return retval;

    return gpioc_request_configure(channel, rflags);
}
