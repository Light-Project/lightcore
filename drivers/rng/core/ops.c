/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/rng.h>
#include <export.h>

#define GENERIC_RNG_OPS(name, operation)        \
state name(struct rng_device *rdev)             \
{                                               \
    struct rng_ops *ops = rdev->ops;            \
                                                \
    if (!ops->operation)                        \
        return -EOPNOTSUPP;                     \
                                                \
    return ops->operation(rdev);                \
}                                               \
EXPORT_SYMBOL(name)

GENERIC_RNG_OPS(rng_enable, enable);
GENERIC_RNG_OPS(rng_disable, disable);

state rng_read(struct rng_device *rdev, unsigned long *data)
{
    struct rng_ops *ops = rdev->ops;

    if (!ops->read)
        return -EOPNOTSUPP;

    return ops->read(rdev, data);
}
EXPORT_SYMBOL(rng_read);

state rng_filled(struct rng_device *rdev, void *data, size_t len)
{
    struct rng_ops *ops = rdev->ops;

    if (!ops->filled)
        return -EOPNOTSUPP;

    return ops->filled(rdev, data, len);
}
EXPORT_SYMBOL(rng_filled);
