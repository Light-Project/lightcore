/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "hlist-debug"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <hlist.h>
#include <panic.h>
#include <export.h>

bool hlist_debug_head_add_check(struct hlist_head *head, struct hlist_node *new)
{
    if (DEBUG_DATA_CHECK(head->node == new,
        "hlist_head_add corruption head->node should not point to new\n"))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_head_add_check);

bool hlist_debug_next_add_check(struct hlist_node *next, struct hlist_node *new)
{
    if (DEBUG_DATA_CHECK(next->next == new,
        "hlist_add corruption next->next should not point to new\n"))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_next_add_check);

bool hlist_debug_prev_add_check(struct hlist_node *prev, struct hlist_node *new)
{
    if (DEBUG_DATA_CHECK(prev->pprev == &new->next,
        "hlist_add corruption prev->next should not point to new\n"))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_prev_add_check);

bool hlist_debug_del_check(struct hlist_node *node)
{
    if (DEBUG_DATA_CHECK(node->next == ERR_PTR(-EFAULT),
        "hlist_del corruption node->next should not point to NULL"))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_del_check);
