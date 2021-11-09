/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <types.h>
#include <stddef.h>

#ifndef __ASSEMBLY__

#define RB_RED    0
#define RB_BLACK  1

typedef struct rb_node {
    struct rb_node *parent;
    struct rb_node *left;
    struct rb_node *right;
    char color;
} rb_node_t;

struct rb_root {
    struct rb_node *rb_node;
};

#define RB_ROOT(name) \
    struct rb_root name = {NULL};

#define	rb_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define rb_first_entry(ptr, type, member) \
    rb_entry(rb_first(ptr), type, member)

#define rb_next_entry(pos, member) \
    rb_entry(rb_next(&(pos)->member), typeof(*(pos)), member)

#define rb_for_each_entry(pos, head, member)                    \
    for (pos = rb_first_entry(head, typeof(*pos), member);      \
        &pos->member != NULL;                                   \
        pos = rb_next_entry(pos, member))

extern void rb_fixup(struct rb_root *root, struct rb_node *node);
extern void rb_del(const struct rb_root *root, struct rb_node *node);

extern struct rb_node *rb_first(const struct rb_root *root);
extern struct rb_node *rb_last(const struct rb_root *root);
extern struct rb_node *rb_prev(const struct rb_node *node);
extern struct rb_node *rb_next(const struct rb_node *node);

extern struct rb_node *rb_pre_first(const struct rb_root *root);
extern struct rb_node *rb_pre_next(const struct rb_node *node);

extern struct rb_node *rb_post_first(const struct rb_root *root);
extern struct rb_node *rb_post_next(const struct rb_node *node);

static inline void rb_link(struct rb_node *parent, struct rb_node **link,
                           struct rb_node *node)
{
    /* link = &parent->left/right */
    *link = node;

    node->parent = parent;
    node->left = node->right = NULL;
}

/**
 * rb_find - find @key in tree @root
 * @root: rb_tree to search
 * @key: key to match
 * @cmp: operator defining the node order
 */
static __always_inline struct rb_node *
rb_find(const struct rb_root *root, const void *key,
        int (*cmp)(const struct rb_node *, const void *key))
{
    struct rb_node *node = root->rb_node;
    int ret;

    while (node) {
        ret = cmp(node, key);

        if (ret < 0)
            node = node->left;
        else if(ret > 0)
            node = node->right;
        else
            return node;
    }

    return NULL;
}

/**
 * rb_find_first - find the first @key in tree @root
 * @root: rb_tree to search
 * @key: key to match
 * @cmp: operator defining the node order
 */
static inline struct rb_node *
rb_find_first(const struct rb_root *root, const void *key,
              int (*cmp)(const struct rb_node *, const void *key))
{
    struct rb_node *node = root->rb_node;
    struct rb_node *match = NULL;
    int ret;

    while (node) {
        ret = cmp(key, node);

        if (ret <= 0) {
            if (!ret)
                match = node;
            node = node->left;
        } else if (ret > 0) {
            node = node->right;
        }
    }

    return match;
}

/**
 * rb_next_match - find the next @key by @node
 * @node: node by search
 * @key: key to match
 * @cmp: operator defining the node order
 */
static inline struct rb_node *
rb_next_match(struct rb_node *node, const void *key,
              int (*cmp)(const void *key, const struct rb_node *))
{
    node = rb_next(node);

    if (node && cmp(key, node))
        node = NULL;

    return node;
}

/**
 * rb_for_each() - iterates a subtree matching @key
 * @node: iterator
 * @tree: tree to search
 * @key: key to match
 * @cmp: operator defining the node order
 */
#define rb_for_each(node, tree, key, cmp)               \
    for (node = rb_find_first((key), (tree), (cmp));    \
        (node); rb_next_match((key), (node), (cmp)))

#endif	/* __ASSEMBLY__ */
#endif  /* _RBTREE_H_ */
