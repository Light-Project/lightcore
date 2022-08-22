/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <btree.h>
#include <proc.h>
#include <kmalloc.h>
#include <export.h>

#define BLOCK_SIZE (CACHE_LINE_SIZE > 128 ? CACHE_LINE_SIZE : 128)
#define NODE_SIZE (BLOCK_SIZE - sizeof(struct btree_node))
#define UINTPTR_PER_U32 DIV_ROUND_UP(BYTES_PER_U32, BYTES_PER_UINTPTR)
#define UINTPTR_PER_U64 DIV_ROUND_UP(BYTES_PER_U64, BYTES_PER_UINTPTR)

struct btree_layout btree_layout32 = {
    .keylen = UINTPTR_PER_U32,
    .keynum = NODE_SIZE / sizeof(uintptr_t) / (UINTPTR_PER_U32 + 1),
    .ptrindex = UINTPTR_PER_U32 * (NODE_SIZE / BYTES_PER_UINTPTR / (UINTPTR_PER_U32 + 1)),
    .nodesize = NODE_SIZE,
};
EXPORT_SYMBOL(btree_layout32);

struct btree_layout btree_layout64 = {
    .keylen = UINTPTR_PER_U64,
    .keynum = NODE_SIZE / sizeof(uintptr_t) / (UINTPTR_PER_U64 + 1),
    .ptrindex = UINTPTR_PER_U64 * (NODE_SIZE / BYTES_PER_UINTPTR / (UINTPTR_PER_U64 + 1)),
    .nodesize = NODE_SIZE,
};
EXPORT_SYMBOL(btree_layout64);

struct btree_layout btree_layoutptr = {
    .keylen = 1,
    .keynum = NODE_SIZE / sizeof(uintptr_t) / 2,
    .ptrindex = NODE_SIZE / sizeof(uintptr_t) / 2,
    .nodesize = NODE_SIZE,
};
EXPORT_SYMBOL(btree_layoutptr);

long btree_keyfind(struct btree_root *root, uintptr_t *node, uintptr_t *key)
{
    struct btree_layout *layout = root->layout;
    unsigned int index;

    for (index = 0; index < layout->keylen; ++index) {
        if (node[index] < key[index])
            return -1;
        if (node[index] > key[index])
            return 1;
    }

    return 0;
}
EXPORT_SYMBOL(btree_keyfind);

void *btree_alloc(struct btree_root *root, gfp_t flags)
{
    struct btree_layout *layout = root->layout;
    return kmalloc(layout->nodesize, flags);
}
EXPORT_SYMBOL(btree_alloc);

void btree_free(struct btree_root *root, void *node)
{
    kfree(node);
}
EXPORT_SYMBOL(btree_free);
