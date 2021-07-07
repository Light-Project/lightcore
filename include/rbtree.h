/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  lightcore/include/rbtree.h
 *  light core rbtree lib
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-01-20      first version 
 * 
 */

#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

#ifndef __ASSEMBLY__

typedef struct rb_node{
    struct rb_node *parent;
    struct rb_node *left;
    struct rb_node *right;
    char color;
} rb_node_t;

struct rb_root{
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
    for (pos = rb_first_entry(head, typeof(*pos), member);    \
        &pos->member != NULL;                                   \
        pos = rb_next_entry(pos, member))

void rb_fixup(struct rb_root *root, struct rb_node *node);
void rb_delete(const struct rb_root *root, struct rb_node *node);

struct rb_node *rb_first(const struct rb_root *root);
struct rb_node *rb_last(const struct rb_root *root);
struct rb_node *rb_prev(const struct rb_node *node);
struct rb_node *rb_next(const struct rb_node *node);

struct rb_node *rb_postorder_next(const struct rb_node *node);



/**
 * rb_find() - find @key in tree @root
 * @key: key to match
 * @root: rb_tree to search
 * @cmp: operator defining the node order
 * 
 * @return: Matching nodes
 */
static __always_inline struct rb_node *
rb_find(const void *key, const struct rb_root *root,
    int (*cmp)(const void *key, const struct rb_node *))
{
    struct rb_node *node = root->rb_node;
    int ret;

    while(node)
    {
        ret = cmp(key, node);

        if(ret < 0)
            node = node->left;
        else if(ret > 0)
            node = node->right;
        else
            return node;
    }
}

/**
 * rb_find_first() - find the first @key in tree @root
 * @key: key to match
 * @root: rb_tree to search
 * @cmp: operator defining the node order
 * 
 * @return: Matching nodes
 */
static __always_inline struct rb_node *
rb_find_first(const void *key, const struct rb_root *root,
    int (*cmp)(const void *key, const struct rb_node *))
{
    struct rb_node *node = root->rb_node;
    struct rb_node *match = NULL;
    int ret;

    while(node)
    {
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
 * rb_next_match() - find the next @key by @node
 * @key: key to match
 * @node: node by search
 * @cmp: operator defining the node order
 * 
 * @return: Matching nodes
 */
static __always_inline struct rb_node *
rb_next_match(const void *key, struct rb_node *node,
    int (*cmp)(const void *key, const struct rb_node *))
{
    node = rb_next(node);
    if(node && cmp(key, node))
        node = NULL;
    return node;
}

/**
 * rb_for_each() - iterates a subtree matching @key
 * @key: key to match
 * @node: iterator
 * @tree: tree to search
 * @cmp: operator defining the node order
 * 
 * @return: Matching nodes
 */
#define rb_for_each(node, key, tree, cmp)               \
    for(                                                \
        node = rb_find_first((key), (tree), (cmp));     \
        (node); rb_next_match((key), (node), (cmp))     \
    )

#endif	/* __ASSEMBLY__ */
#endif