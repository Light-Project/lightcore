/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "res"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <spinlock.h>
#include <resource.h>
#include <panic.h>
#include <export.h>

static RB_ROOT(resource_sort);
static LIST_HEAD(resource_root);
static SPIN_LOCK(resource_lock);

static long resource_rb_find(const struct rb_node *rb, const void *key)
{
    const struct resource *node = rb_to_resource(rb);
    const struct resource *cmp = key;

    if (resource_type(node) == resource_type(cmp) && !(
        resource_end(cmp) < node->start || resource_end(node) < cmp->start))
        return 0;

    if (node->type != cmp->type)
        return node->type > cmp->type ? -1 : 1;

    if (node->parent != cmp->parent)
        return node->parent > cmp->parent ? -1 : 1;

    if (node->start != cmp->start)
        return node->start > cmp->start ? -1 : 1;

    if (node->size != cmp->size)
        return node->size > cmp->size ? -1 : 1;

    return LONG_MIN;
}

struct resource *resource_register_conflict(struct resource *parent, struct resource *res)
{
    struct rb_node *node, *rbparent, **rblink;
    struct resource *find;

    spin_lock(&resource_lock);

    node = rb_find_last(&resource_sort, res, resource_rb_find, &rbparent, &rblink);
    if (unlikely(find = rb_to_resource_safe(node)))
        goto exit;

    res->parent = parent;
    list_head_init(&res->child);
    list_add(&parent->child, &res->siblings);
    rb_insert_node(&resource_sort, rbparent, rblink, &res->sort);

exit:
    spin_unlock(&resource_lock);
    return find;
}
EXPORT_SYMBOL(resource_register_conflict);

state resource_register(struct resource *parent, struct resource *res)
{
    struct resource *conflict;
    conflict = resource_register_conflict(parent, res);
    return conflict ? -EBUSY : -ENOERR;
}
EXPORT_SYMBOL(resource_register);

state resource_unregister(struct resource *res)
{
    state retval = -ENOERR;

    spin_lock(&resource_lock);

    if (!list_check_empty(&res->child))
        retval = -EBUSY;
    else {
        list_del(&res->siblings);
        rb_delete(&resource_sort, &res->sort);
    }

    spin_unlock(&resource_lock);
    return retval;
}
EXPORT_SYMBOL(resource_unregister);
