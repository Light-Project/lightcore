/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "rbtree-debug"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <rbtree.h>
#include <datacheck.h>
#include <export.h>

bool rb_debug_link_check(struct rb_node *parent, struct rb_node **link, struct rb_node *node)
{
    if (DEBUG_DATA_CHECK(*link == node,
        "rb_insert corruption (%p) *link should not be node (%p)\n",
        link, node))
        return false;

    return true;
}
EXPORT_SYMBOL(rb_debug_link_check);

bool rb_debug_delete_check(struct rb_node *node)
{
    if (DEBUG_DATA_CHECK(node->left == POISON_RBNODE1,
        "rb_delete corruption (%p) node->left should not be POISON_RBNODE1 (%p)\n",
        node, POISON_RBNODE1))
        return false;

    if (DEBUG_DATA_CHECK(node->right == POISON_RBNODE2,
        "rb_delete corruption (%p) node->right should not be POISON_RBNODE2 (%p)\n",
        node, POISON_RBNODE2))
        return false;

    if (DEBUG_DATA_CHECK(node->parent == POISON_RBNODE3,
        "rb_delete corruption (%p) node->parent should not be POISON_RBNODE3 (%p)\n",
        node, POISON_RBNODE3))
        return false;

    return true;
}
EXPORT_SYMBOL(rb_debug_delete_check);
