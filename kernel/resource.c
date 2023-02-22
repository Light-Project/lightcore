/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "resource"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <resource.h>
#include <spinlock.h>
#include <ioops.h>
#include <export.h>

struct resource root_mmio = {
    .name = "root mmio",
    .start = 0,
    .size = -1,
    .type = RESOURCE_MMIO,
};
EXPORT_SYMBOL(root_mmio);

struct resource root_pmio = {
    .name = "root pmio",
    .start = 0,
    .size = IO_SPACE_LIMIT,
    .type = RESOURCE_PMIO,
};
EXPORT_SYMBOL(root_pmio);

static SPIN_LOCK(resource_lock);

static struct resource *request_resource(struct resource *parent, struct resource *res)
{
    struct list_head *head;
    struct resource *walk;

    if (!res->size)
        return ERR_PTR(-EINVAL);

    if (res->start < parent->start ||
        resource_end(res) > resource_end(parent))
        return ERR_PTR(-EADDRNOTAVAIL);

    head = &parent->child;
    list_for_each_entry(walk, &parent->child, siblings) {
        if (walk->start > res->start)
            break;
        if (resource_end(walk) > resource_end(res))
            return walk;
        head = &walk->siblings;
    }

    res->parent = parent;
    list_add(head, &res->siblings);

    return NULL;
}

static state release_resource(struct resource *res)
{
    struct resource *parent = res->parent;
    struct resource *walk, *find;

    list_for_each_entry(walk, &parent->child, siblings) {
        if (walk == res) {
            find = walk;
            break;
        }
    }

    if (unlikely(!find))
        return -ENOENT;

    res->parent = NULL;
    list_del(&res->siblings);

    return -ENOERR;
}

struct resource *resource_request_conflict(struct resource *parent, struct resource *res)
{
    struct resource *find;

    spin_lock(&resource_lock);
    find = request_resource(parent, res);
    spin_unlock(&resource_lock);

    return find;
}
EXPORT_SYMBOL(resource_request_conflict);

state resource_request(struct resource *parent, struct resource *res)
{
    struct resource *conflict;

    conflict = resource_request_conflict(parent, res);
    if (IS_ERR(conflict))
        return PTR_ERR(conflict);

    return conflict ? -EBUSY : -ENOERR;
}
EXPORT_SYMBOL(resource_request);

state resource_release(struct resource *res)
{
    state retval;

    spin_lock(&resource_lock);
    retval = release_resource(res);
    spin_unlock(&resource_lock);

    return retval;
}
EXPORT_SYMBOL(resource_release);
