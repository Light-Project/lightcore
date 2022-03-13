/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "list-debug"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <list.h>
#include <panic.h>
#include <export.h>

bool list_debug_add_check(struct list_head *prev, struct list_head *next, struct list_head *new)
{
    if (DEBUG_DATA_CHECK(prev->next != next,
        "list_add corruption prev->next should point to next\n"))
        return false;

    if (DEBUG_DATA_CHECK(next->prev != prev,
        "list_add corruption next->prev should point to prev\n"))
        return false;

    if (DEBUG_DATA_CHECK(new->next == next,
        "list_add corruption new->next should not point to next\n"))
        return false;

    if (DEBUG_DATA_CHECK(new->prev == prev,
        "list_add corruption new->prev should not point to prev\n"))
        return false;

    return true;
}
EXPORT_SYMBOL(list_debug_add_check);

bool list_debug_del_check(struct list_head *node)
{
    if (DEBUG_DATA_CHECK(node->next == NULL,
        "list_del corruption node->next should not be NULL\n"))
        return false;

    if (DEBUG_DATA_CHECK(node->prev == NULL,
        "list_del corruption node->prev should not be NULL\n"))
        return false;

    return true;
}
EXPORT_SYMBOL(list_debug_del_check);
