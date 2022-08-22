/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BTREE_H_
#define _BTREE_H_

#include <types.h>
#include <slist.h>

struct btree_root;
typedef void *(*btree_alloc_t)(struct btree_root *root, gfp_t flags);
typedef void (*btree_free_t)(struct btree_root *root, void *block);
typedef long (*btree_find_t)(struct btree_root *root, uintptr_t *node, uintptr_t *key);
typedef state (*btree_clash_t)(struct btree_root *root, void *clash, void *value);
typedef void *(*btree_remove_t)(struct btree_root *root, void *value);

struct btree_layout {
    unsigned int keylen;
    unsigned int keynum;
    unsigned int ptrindex;
    size_t nodesize;
};

struct btree_root {
    struct btree_layout *layout;
    struct btree_node *node;
    unsigned int height;

    btree_alloc_t alloc;
    btree_free_t free;
    btree_find_t find;
    btree_clash_t clash;
    btree_remove_t remove;
    void *pdata;
};

struct btree_node {
    uintptr_t block[0];
};

#define BTREE_ROOT_STATIC(_layout, _alloc, _free, _find,    \
                          _clash, _remove, _pdata) {        \
    .layout = _layout, .alloc = _alloc,                     \
    .free = _free, .find = _find, .clash = _clash,          \
    .remove = _remove, .pdata = _pdata,                     \
}

#define BTREE_ROOT_INIT(layout, alloc, free, find,          \
                        clash, remove, pdata)               \
    (struct btree_root) BTREE_ROOT_STATIC(                  \
        layout, alloc, free, find, clash, remove, pdata)

#define BTREE_ROOT(name, layout, alloc, free, find,         \
                   clash, remove, pdata)                    \
    struct btree_root name = BTREE_ROOT_INIT(               \
        layout, alloc, free, find, clash, remove, pdata)

extern struct btree_layout btree_layout32;
extern struct btree_layout btree_layout64;
extern struct btree_layout btree_layoutptr;

extern long btree_keyfind(struct btree_root *root, uintptr_t *node, uintptr_t *key);
extern void *btree_alloc(struct btree_root *root, gfp_t flags);
extern void btree_free(struct btree_root *root, void *node);

extern void *btree_lookup(struct btree_root *root, uintptr_t *key);
extern state btree_insert(struct btree_root *root, uintptr_t *key, void *value, gfp_t flags);
extern void *btree_remove(struct btree_root *root, uintptr_t *key);

#endif  /* _BTREE_H_ */
