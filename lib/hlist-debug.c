/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "hlist-debug"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <hlist.h>
#include <datacheck.h>
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
    if (DEBUG_DATA_CHECK(new == next,
        "hlist_next_add corruption double add: new=(%p), next=(%p)\n",
        new, next))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_next_add_check);

bool hlist_debug_prev_add_check(struct hlist_node *prev, struct hlist_node *new)
{
    if (DEBUG_DATA_CHECK(new == prev,
        "hlist_prev_add corruption double add: new=(%p), prev=(%p)\n",
        new, prev))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_prev_add_check);

bool hlist_debug_del_check(struct hlist_node *node)
{
    if (DEBUG_DATA_CHECK(node->next == POISON_HLIST1,
        "hlist_del corruption (%p) node->next should not be POISON_HLIST1 (%p)",
        node, POISON_HLIST1))
        return false;

    if (DEBUG_DATA_CHECK(node->pprev == POISON_HLIST2,
        "hlist_del corruption (%p) node->pprev should not be POISON_HLIST2 (%p)",
        node, POISON_HLIST2))
        return false;

    return true;
}
EXPORT_SYMBOL(hlist_debug_del_check);
