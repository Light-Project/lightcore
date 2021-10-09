/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <initcall.h>
#include <resource.h>
#include <export.h>

static LIST_HEAD(resource_list);

state resource_register(struct resource *resource)
{
    if(!resource)
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

static state resource_init(void)
{
    return -ENOERR;
}
fs_initcall(resource_init);
