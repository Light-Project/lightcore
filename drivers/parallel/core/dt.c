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
    state retval;

    pdev = parallel_device_alloc(host, NULL);
    if (unlikely(!pdev)) {
        return -ENOMEM;
    }

    pdev->dt_node = node;
    pdev->dev.name = node->path;
    pdev->dev.fwnode = &node->fwnode;

    retval = parallel_device_register(pdev);
    if (retval)
        goto failed;

    return -ENOERR;

failed:
    parallel_device_free(pdev);
    return retval;
}

void dt_populate_parallel(struct parallel_host *host)
{
    struct dt_node *walk, *node;
    state ret;

    if (!fwnode_test_dtree(host->dev->fwnode))
        return;

    node = device_node_container(host->dev);
    dt_for_each_child(walk, node) {
        ret = dt_register_parallel_device(host, walk);
        if (ret) {
            pr_warn("failed to create device for %p\n", walk);
        }
    }
}
