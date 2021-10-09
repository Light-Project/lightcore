/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/parallel.h>

state parallel_register(struct parallel_device *pdev)
{
    if (!pdev->ops)
        return -EINVAL;

    return -ENOERR;
}

void parallel_unregister(struct parallel_device *pdev)
{
    list_del(&pdev->list);
}
