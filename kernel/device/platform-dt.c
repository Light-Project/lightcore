/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"pdt: " fmt

#include <mm.h>
#include <init/initcall.h>
#include <driver/platform.h> 
#include <driver/dt.h> 
#include "base.h"
#include <printk.h>

static const struct dt_device_id dt_bus_table[] = {
    { .compatible = "simple-bus", },
    { .compatible = "amba-bus", },
    { .compatible = "isa-bus", },
    { }, /* NULL */
};

static const struct dt_device_id dt_skipped_table[] = {
    { .compatible = "operating-points-v2", },
    { }, /* NULL */
};

static inline const struct dt_device_id * 
dt_device_match(const struct dt_device_id *ids, struct dt_node *node)
{
    while((ids->name && ids->name[0]) || (ids->type && ids->type[0]) ||
          (ids->compatible && ids->compatible[0])) {
        if(dt_match(ids, node))
            return ids;
        ++ids;
    }
    return NULL;
}

const struct dt_device_id * 
platform_dt_match(struct platform_driver *pdrv, 
                  struct platform_device *pdev)
{
    if(!pdrv->dt_table || !pdev->dt_node)
        return NULL;
    return dt_device_match(pdrv->dt_table, pdev->dt_node);
}

static __always_inline struct platform_device *
pdt_alloc_node(struct dt_node *node)
{
    struct platform_device *pdev;

    pdev = platform_device_alloc(NULL, 0);
    if(!pdev)
        return NULL;

    pdev->dt_node = node;
	return pdev;
}

static inline state
pdt_populate_resource(struct platform_device *pdev)
{
    struct resource *res;
    int count, res_nr;

    res_nr = dt_address_nr(pdev->dt_node);
    res = kzalloc(sizeof(*res) * res_nr, GFP_KERNEL);
    if (!res)
        return -ENOMEM;

    for (count = 0; count < res_nr; ++count) {
    }

    pdev->resource = res;
    pdev->resources_nr = res_nr;

    return -ENOERR;
}

static state platform_dt_setup_node(struct dt_node *node)
{
	struct platform_device *pdev;

    pdev = pdt_alloc_node(node);
    if(!pdev)
        return -ENOMEM;

    pr_debug("New device: %s\n", node->path);
    // pdt_populate_resource(pdev);

    return device_register(&pdev->device);
}

state platform_dt_setup_bus(struct dt_node *bus, 
                            const struct dt_device_id *matches)
{
    struct dt_node *node;

    if(!dt_attribute_get(bus, "compatible"))
        return -ENOERR;

    if(unlikely(dt_match(dt_skipped_table, bus)))
        return -ENOERR;

    if(!dt_device_match(matches, bus))
        return platform_dt_setup_node(bus);

    dt_for_each_child(node, bus) {
        if(platform_dt_setup_bus(node, matches))
            break;
    }

    return -ENOERR;
}

void __init platform_dt_init(void)
{
    struct dt_node *bus;

    if (!dt_root)
        return;

    dt_for_each_child(bus, dt_root)
        platform_dt_setup_bus(bus, dt_bus_table);
}
