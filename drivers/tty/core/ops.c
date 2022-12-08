/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/tty.h>
#include <export.h>

#define GENERIC_TTY_OPS(operation, rtype, errval, args, ...)    \
rtype tty_##operation(struct tty_device *tdev, ##__VA_ARGS__)   \
{                                                               \
    struct tty_ops *ops = tdev->ops;                            \
                                                                \
    if (!ops->operation)                                        \
        return errval;                                          \
                                                                \
    return ops->operation args;                                 \
}                                                               \
EXPORT_SYMBOL(tty_##operation)

GENERIC_TTY_OPS
GENERIC_TTY_OPS
GENERIC_TTY_OPS
GENERIC_TTY_OPS
GENERIC_TTY_OPS
