/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <export.h>
#include <mm.h>
#include <device.h>
#include <resource.h>

static LIST_HEAD(resource_list);

static bool resource_already(struct resource *resource)
{
    struct resource *walk;

    list_for_each_entry(walk, &resource_list, list)
    if ((walk->start < resource->start && resource->start < walk->end) ||
        (walk->start < resource->end && resource->end < walk->end)     ||
        (resource->start < walk->start && walk->end < resource->end)   ||
        (walk->start < resource->start && resource->end < walk->end))
        return true;

    return false;
}

state resource_register(struct resource *resource)
{
    if(!resource || resource_already(resource))
        return -EINVAL;

    list_add_prev(&resource_list, &resource->list);
    return -ENOERR;
}
EXPORT_SYMBOL(resource_register);

void resource_unregister(struct resource *resource)
{
    list_del(&resource->list);
}
EXPORT_SYMBOL(resource_unregister);

static inline void devres_add(struct device *dev, struct resource *res)
{

}

static inline void devres_remove(struct device *dev, struct resource *res)
{

}

void *dev_kmalloc(size_t size, gfp_t gfp)
{

    return NULL;
}

void dev_kfree(void *block)
{

}

void *dev_ioremap(resource_size_t addr, resource_size_t size)
{

    return NULL;
}

void dev_iounmap(resource_size_t addr, resource_size_t size)
{

}

void devres_release_all(void)
{

}
