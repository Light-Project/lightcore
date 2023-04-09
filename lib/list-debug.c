/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "list-debug"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <list.h>
#include <datacheck.h>
#include <export.h>

bool list_debug_add_check(struct list_head *prev, struct list_head *next, struct list_head *new)
{
    if (DEBUG_DATA_CHECK(prev->next != next,
        "list_add corruption (%p) prev->next should be next (%p), but was (%p)\n",
        prev, next, prev->next))
        return false;

    if (DEBUG_DATA_CHECK(next->prev != prev,
        "list_add corruption (%p) next->prev should be prev (%p), but was (%p)\n",
        next, prev, next->prev))
        return false;

    if (DEBUG_DATA_CHECK(new == prev || new == next,
        "list_add corruption double add: new=(%p), prev=(%p), next=(%p)\n",
        new, prev, next))
        return false;

    return true;
}
EXPORT_SYMBOL(list_debug_add_check);

bool list_debug_del_check(struct list_head *node)
{
    if (DEBUG_DATA_CHECK(node->next == POISON_LIST1,
        "list_del corruption (%p) node->next should not be POISON_LIST1 (%p)\n",
        node, POISON_LIST1))
        return false;

    if (DEBUG_DATA_CHECK(node->prev == POISON_LIST2,
        "list_del corruption (%p) node->prev should not be POISON_LIST2 (%p)\n",
        node, POISON_LIST2))
        return false;

    return true;
}
EXPORT_SYMBOL(list_debug_del_check);
