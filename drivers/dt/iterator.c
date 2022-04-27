/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "dt-iter"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <driver/dt.h>
#include <printk.h>
#include <export.h>

struct dt_node *dt_iterative_all(struct dt_node *node)
{
    if (!node)
        return dt_root;
    else if (slist_check_next(&node->child))
        return slist_first_entry(&node->child, struct dt_node, sibling);
    else {
        while (!slist_check_next(&node->sibling) && node->parent)
            node = node->parent;
        return slist_next_entry(node, sibling);
    }
}
EXPORT_SYMBOL(dt_iterative_all);

struct dt_node *dt_iterative_all_available(struct dt_node *node)
{
    for (node = dt_iterative_all(node);
         node && !dt_node_check_available(node);
         node = dt_iterative_all(node));
    return node;
}
EXPORT_SYMBOL(dt_iterative_all_available);

struct dt_node *dt_iterative_child(struct dt_node *node, struct dt_node *iter)
{
    if (!node)
        return NULL;
    else if (iter)
        return slist_next_entry(iter, sibling);
    else
        return slist_first_entry(&node->child, struct dt_node, sibling);
}
EXPORT_SYMBOL(dt_iterative_child);

struct dt_node *dt_iterative_child_available(struct dt_node *node, struct dt_node *iter)
{
    for (iter = dt_iterative_child(node, iter);
         iter && !dt_node_check_available(node);
         iter = dt_iterative_child(node, iter));
    return node;
}
EXPORT_SYMBOL(dt_iterative_child_available);

state dt_iterative_phandle_init(struct dt_phandle_iterator *iter, const struct dt_node *node,
                                const char *listn, const char *celln, int cellc)
{
    const be32 *list;
    int size;

    if (!celln && cellc < 0)
        return -EINVAL;

    list = dt_attribute_get(node, listn, &size);
    if (!list)
        return -ENOENT;

    iter->cells_name = celln;
    iter->cell_count = cellc;

    iter->parent = node;
    iter->curr = iter->phandle_end = list;
    iter->list_end = (void *)list + size;

    return -ENOERR;
}
EXPORT_SYMBOL(dt_iterative_phandle_init);

state dt_iterative_phandle_next(struct dt_phandle_iterator *iter)
{
    uint32_t count = 0;

    if (iter->phandle_end >= iter->list_end || !iter->curr)
        return -ENOENT;

    iter->curr = iter->phandle_end;
    iter->phandle = be32_to_cpup(iter->curr++);

    if (!iter->phandle)
        goto finish;

    iter->node = dt_find_by_phandle(iter->phandle);
    if (!iter->cells_name)
        count = iter->cell_count;

    else {
        if (!iter->node) {
            pr_err("node %p could not find phandle %d\n",
                    iter->parent, iter->phandle);
            goto error;
        }

        if (dt_attribute_read_u32(iter->node, iter->cells_name, &count)) {
            if (iter->cell_count >= 0)
                count = iter->cell_count;
            else {
                pr_err("node %p could not get %s for %p\n",
                        iter->parent, iter->cells_name, iter->node);
                goto error;
            }
        }
    }

    if (unlikely(iter->curr + count > iter->list_end)) {
        if (iter->cells_name)
            pr_err("node %p %s = %d found %ld\n",
                    iter->parent, iter->node->name,
                    count, (long)(iter->list_end - iter->curr));
        else
            pr_err("node %p phandle %s needs %d, found %ld\n",
                    iter->parent, iter->node->name,
                    count, (long)(iter->list_end - iter->curr));
        goto error;
    }

finish:
    iter->phandle_end = iter->curr + count;
    iter->count = count;
    return -ENOERR;

error:
    iter->node = NULL;
    return -EINVAL;
}
EXPORT_SYMBOL(dt_iterative_phandle_next);

int dt_iterative_phandle_args(struct dt_phandle_iterator *iter, uint32_t *args, int size)
{
    int index, count = iter->count;

    if (size < count) {
        pr_err("node %p arg number %d exceeds buffer %d\n",
                iter->parent, count, size);
        count = size;
    }

    for (index = 0; index < count; ++index) {
        args[index] = be32_to_cpup(iter->curr++);
    }

    return count;
}
EXPORT_SYMBOL(dt_iterative_phandle_args);
