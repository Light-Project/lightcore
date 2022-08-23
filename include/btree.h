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
extern state btree_update(struct btree_root *root, uintptr_t *key, void *value);
extern state btree_insert(struct btree_root *root, uintptr_t *key, void *value, gfp_t flags);
extern void *btree_remove(struct btree_root *root, uintptr_t *key);
extern void btree_destroy(struct btree_root *root);

extern void btree_key_copy(struct btree_root *root, uintptr_t *dest, uintptr_t *src);
extern void *btree_first(struct btree_root *root, uintptr_t *key);
extern void *btree_last(struct btree_root *root, uintptr_t *key);
extern void *btree_next(struct btree_root *root, uintptr_t *key);
extern void *btree_prev(struct btree_root *root, uintptr_t *key);

/**
 * btree_for_each - iterate over a btree.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 */
#define btree_for_each(root, key, value)                    \
    for (value = btree_first(root, key);                    \
         value; value = btree_next(root, key))

/**
 * btree_for_each_reverse - iterate over a btree backwards.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 */
#define btree_for_each_reverse(root, key, value)            \
    for (value = btree_last(root, key);                     \
         value; value = btree_prev(root, key))

/**
 * btree_for_each_from - iterate over a btree from the current point.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 */
#define btree_for_each_from(root, key, value)               \
    for (; value; value = btree_next(root, key))

/**
 * btree_for_each_reverse_from - iterate over a btree backwards from the current point.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 */
#define btree_for_each_reverse_from(root, key, value)       \
    for (; value; value = btree_prev(root, key))

/**
 * btree_for_each_continue - continue iteration over a btree.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 */
#define btree_for_each_continue(root, key, value)           \
    for (value = btree_next(root, key);                     \
         value; value = btree_next(root, key))

/**
 * btree_for_each_reverse_continue - continue iteration over a btree backwards.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 */
#define btree_for_each_reverse_continue(root, key, value)   \
    for (value = btree_prev(root, key);                     \
         value; value = btree_prev(root, key))

/**
 * btree_for_each_safe - iterate over btree safe against removal.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 * @tkey: another &key to use as temporary storage.
 * @tval: another value to use as temporary storage.
 */
#define btree_for_each_safe(root, key, value, tkey, tval)   \
    for (value = btree_first(root, key),                    \
         btree_key_copy(root, tkey, key),                   \
         tval = btree_next(root, tkey); value;              \
         btree_key_copy(root, key, tkey),                   \
         value = tval, tval = btree_next(root, tkey))

/**
 * btree_for_each_reverse_safe - iterate backwards over btree safe against removal.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 * @tkey: another &key to use as temporary storage.
 * @tval: another value to use as temporary storage.
 */
#define btree_for_each_reverse_safe(root, key, value, tkey, tval)   \
    for (value = btree_last(root, key),                             \
         btree_key_copy(root, tkey, key),                           \
         tval = btree_prev(root, tkey); value;                      \
         btree_key_copy(root, key, tkey),                           \
         value = tval, tval = btree_prev(root, tkey))

/**
 * btree_for_each_from_safe - iterate over btree from current point safe against removal.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 * @tkey: another &key to use as temporary storage.
 * @tval: another value to use as temporary storage.
 */
#define btree_for_each_from_safe(root, key, value, tkey, tval)      \
    for (btree_key_copy(root, tkey, key),                           \
         tval = btree_next(root, tkey); value;                      \
         btree_key_copy(root, key, tkey),                           \
         value = tval, tval = btree_next(root, tkey))

/**
 * btree_for_each_reverse_from_safe - iterate backwards over btree from current point safe against removal.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 * @tkey: another &key to use as temporary storage.
 * @tval: another value to use as temporary storage.
 */
#define btree_for_each_reverse_from_safe(root, key, value, tkey, tval)  \
    for (btree_key_copy(root, tkey, key),                               \
         tval = btree_prev(root, tkey); value;                          \
         btree_key_copy(root, key, tkey),                               \
         value = tval, tval = btree_prev(root, tkey))

/**
 * btree_for_each_continue_safe - continue btree iteration safe against removal.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 * @tkey: another &key to use as temporary storage.
 * @tval: another value to use as temporary storage.
 */
#define btree_for_each_continue_safe(root, key, value, tkey, tval)      \
    for (value = btree_next(root, key),                                 \
         btree_key_copy(root, tkey, key),                               \
         tval = btree_next(root, tkey); value;                          \
         btree_key_copy(root, key, tkey),                               \
         value = tval, tval = btree_next(root, tkey))

/**
 * btree_for_each_reverse_continue_safe - continue backwards over btree iteration safe against removal.
 * @root: the root for your btree.
 * @key: the &key to use as a loop cursor.
 * @value: the value of current loop cursor.
 * @tkey: another &key to use as temporary storage.
 * @tval: another value to use as temporary storage.
 */
#define btree_for_each_reverse_continue_safe(root, key, value, tkey, tval)  \
    for (value = btree_prev(root, key),                                     \
         btree_key_copy(root, tkey, key),                                   \
         tval = btree_prev(root, tkey); value;                              \
         btree_key_copy(root, key, tkey),                                   \
         value = tval, tval = btree_prev(root, tkey))

#endif  /* _BTREE_H_ */
