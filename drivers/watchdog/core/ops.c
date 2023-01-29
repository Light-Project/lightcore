/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/watchdog.h>
#include <mhelper.h>
#include <export.h>

#define GENERIC_WATCHDOG_OPS(name)                  \
state watchdog_##name(struct watchdog_device *wdev) \
{                                                   \
    struct watchdog_ops *ops = wdev->ops;           \
                                                    \
    if (!ops->name)                                 \
        return -ENXIO;                              \
                                                    \
    return ops->name(wdev);                         \
}                                                   \
EXPORT_SYMBOL(watchdog_##name)

GENERIC_WATCHDOG_OPS(start);
GENERIC_WATCHDOG_OPS(stop);

#define GENERIC_WATCHDOG_NARG(operation, argnr, ...)                            \
state watchdog_##operation                                                      \
(struct watchdog_device *wdev, MMAP_DECLN(argnr, MARGFN_DECL, __VA_ARGS__))     \
{                                                                               \
    struct watchdog_ops *ops = wdev->ops;                                       \
                                                                                \
    if (!ops->operation)                                                        \
        return -EOPNOTSUPP;                                                     \
                                                                                \
    return ops->operation(wdev, MMAP_DECLN(argnr, MARGFN_ARGS, __VA_ARGS__));   \
}                                                                               \
EXPORT_SYMBOL(watchdog_##operation)

#define GENERIC_WATCHDOG_ARG(operation, ...) \
    GENERIC_WATCHDOG_NARG(operation, COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

GENERIC_WATCHDOG_ARG(timeleft_get, unsigned int *);
GENERIC_WATCHDOG_ARG(timeout_get, unsigned int *);
GENERIC_WATCHDOG_ARG(timeout_set, unsigned int );

state watchdog_feed(struct watchdog_device *wdev)
{
    struct watchdog_ops *ops = wdev->ops;
    state retval;

    /* If no feed opt, restart the watchdog */
    if (ops->feed)
        retval = ops->feed(wdev);
    else
        retval = ops->start(wdev);

    return retval;
}
EXPORT_SYMBOL(watchdog_feed);
