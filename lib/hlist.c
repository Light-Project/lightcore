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
        "hlist_head_add corruption (%p) head->node should not be new (%p)\n",
        head, new))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_head_add_check);

bool hlist_debug_next_add_check(struct hlist_node *next, struct hlist_node *new)
{
    if (DEBUG_DATA_CHECK(next->next == new,
        "hlist_next_add corruption (%p) next->next should not be new (%p)\n",
        next, new))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_next_add_check);

bool hlist_debug_prev_add_check(struct hlist_node *prev, struct hlist_node *new)
{
    if (DEBUG_DATA_CHECK(prev->pprev == &new->next,
        "hlist_prev_add corruption (%p) prev->pprev should not be new (%p)\n",
        prev, new))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_prev_add_check);

bool hlist_debug_del_check(struct hlist_node *node)
{
    if (DEBUG_DATA_CHECK(node->next == POISON_HLIST1,
        "hlist_del corruption (%p) node->next should not be POISON_HLIST1 (%p)",
        node, POISON_LIST1))
        return false;

    if (DEBUG_DATA_CHECK(node->pprev == POISON_HLIST2,
        "hlist_del corruption (%p) node->pprev should not be POISON_HLIST2 (%p)",
        node, POISON_LIST2))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_del_check);
