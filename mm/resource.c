/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * IO space allocator
 * 
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 * 
 */

#include <resource.h>

static LIST_HEAD(resource_list);

static bool resource_find(struct resource *res)
{
    struct resource *walk;

    list_for_each_entry(walk, &resource_list, list) {


    }
    return false;
}

state resource_alloc(struct resource *res)
{
    if(!res)
        return -EINVAL;

    if(resource_find(res))
        return -EBUSY;

    list_add_prev(&resource_list, &res->list);

    return -ENOERR;
}

void resource_free(struct resource *res)
{
    list_del(&res->list);
}
