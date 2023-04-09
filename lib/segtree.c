/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <segtree.h>
#include <export.h>

#define SEGTREE_START(segnode) ((segnode)->start)
#define SEGTREE_END(segnode) ((segnode)->end)
SEGTREE_DEFINE(, segtree, struct segtree_node, node, unsigned long, subtree, SEGTREE_START, SEGTREE_END);

EXPORT_SYMBOL(segtree_insert);
EXPORT_SYMBOL(segtree_delete);
EXPORT_SYMBOL(segtree_first);
EXPORT_SYMBOL(segtree_next);
