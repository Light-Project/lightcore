/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "parallel-dt"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/parallel.h>
#include <driver/dt.h>

static state dt_register_parallel_device(struct parallel_host *host, struct dt_node *node)
{
    struct parallel_device *pdev;

    pdev = parallel_device_alloc(host, NULL);
    if (unlikely(!pdev)) {
        return -ENOMEM;
    }

    pdev->dt_node = node;
    return parallel_device_register(pdev);
}

void dt_populate_parallel(struct parallel_host *host)
{
    struct dt_node *node;
    state ret;

    if (!host->dt_node)
        return;

    dt_for_each_child(node, host->dt_node) {
        ret = dt_register_parallel_device(host, node);
        if (ret) {
            pr_warn("failed to create device for %p\n", node);
        }
    }
}
