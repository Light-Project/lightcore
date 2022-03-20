/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "slist-debug"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <slist.h>
#include <panic.h>
#include <export.h>

bool slist_debug_add_check(struct slist_head *node, struct slist_head *new)
{
    if (DEBUG_DATA_CHECK(new->next && new->next == node->next,
        "slist_add corruption (%p) new->next should not be next (%p)\n",
        new, node))
        return false;

    return true;
}
EXPORT_SYMBOL(slist_debug_add_check);

bool slist_debug_del_check(struct slist_head *node)
{
    if (DEBUG_DATA_CHECK(node->next == POISON_SLIST,
        "slist_del corruption (%p) node->next should not be POISON_SLIST (%p)\n",
        node, POISON_SLIST))
        return false;

    return true;
}
EXPORT_SYMBOL(slist_debug_del_check);
