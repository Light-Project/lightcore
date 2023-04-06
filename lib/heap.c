/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <heap.h>
#include <titer.h>
#include <export.h>

static __always_inline void
parent_swap(struct heap_root *root, struct heap_node *parent, struct heap_node *node)
{
    struct heap_node *gparent = parent->parent;
    struct heap_node shadow = *node;

    if (node->left)
        node->left->parent = parent;
    if (node->right)
        node->right->parent = parent;

    if (parent->left == node) {
        node->left = parent;
        if ((node->right = parent->right))
            parent->right->parent = node;
    } else { /* parent->right == node */
        node->right = parent;
        if ((node->left = parent->left))
            parent->left->parent = node;
    }

    if (!(node->parent = gparent))
        root->node = node;
    else if (gparent->left == parent)
        gparent->left = node;
    else /* gparent->right == parent */
        gparent->right = node;

    *parent = shadow;
    parent->parent = node;
}

/**
 * heap_fixup - balance after insert node.
 * @root: heap root of node.
 * @node: new inserted node.
 * @cmp: operator defining the node order.
 */
void heap_fixup(struct heap_root *root, struct heap_node *node, heap_cmp_t cmp)
{
    struct heap_node *parent;

    while ((parent = node->parent)) {
        if (cmp(node, parent) >= 0)
            break;
        parent_swap(root, parent, node);
    }
}
EXPORT_SYMBOL(heap_fixup);

/**
 * heap_erase - balance after remove node.
 * @root: heap root of node.
 * @node: removed node.
 * @cmp: operator defining the node order.
 */
void heap_erase(struct heap_root *root, struct heap_node *node, heap_cmp_t cmp)
{
    struct heap_node *successor = node->parent;
    struct heap_node *child1, *child2;
    long result;

    if (successor && (result = cmp(node, successor)) < 0)
        heap_fixup(root, node, cmp);

    else for (;;) {
        child1 = node->left;
        child2 = node->right;

        if (!child1 && !child2)
            return;
        else if (!child2)
            successor = node->left;
        else if (!child1)
            successor = node->left;
        else { /* child1 && child2 */
            if (cmp(node->left, node->right) < 0)
                successor = node->left;
            else
                successor = node->right;
        }

        if (cmp(node, successor) < 0)
            return;
        parent_swap(root, node, successor);
    }
}
EXPORT_SYMBOL(heap_erase);

/**
 * heap_remove - remove node form heap.
 * @root: heap root of node.
 * @node: node to remove.
 */
struct heap_node *heap_remove(struct heap_root *root, struct heap_node *node)
{
    struct heap_node *successor = heap_find(root, root->count);

    if (!successor->parent) {
        /*
         * Case 1: node to delete is root.
         *
         *   (r)      (p)
         *    |        |
         *   (n)  ->  null
         *
         */

        root->node = NULL;
        successor = NULL;
    } else {
        struct heap_node *parent;

        parent = successor->parent;
        if (parent->left == successor)
            parent->left = NULL;
        else /* parent->right == successor */
            parent->right = NULL;

        /*
         * Case 2: node to delete is successor.
         *
         *     (p)          (p)
         *     / \          /
         *   (s) (n)  ->  (s)
         *
         */

        if (node == successor) {
            successor = NULL;
            goto finish;
        }

        /*
         * Case 3: node to delete is normal.
         *
         *         (p)              (p)
         *        /   \             / \
         *       /     \          (x) (s)
         *     (n)     (s)  ->    / \
         *     / \     /        (z) (y)
         *   (x) (y) (z)
         *
         */

        /* inheritance information */
        *successor = *node;

        /* inheritance relationship */
        if (node->left)
            node->left->parent = successor;
        if (node->right)
            node->right->parent = successor;

        if (!(parent = node->parent))
            root->node = successor;
        else {
            if (node == parent->left)
                parent->left = successor;
            else if (node == parent->right)
                parent->right = successor;
        }
    }

finish:
    root->count--;
    return successor;
}
EXPORT_SYMBOL(heap_remove);

/**
 * heap_parent - find the parent node.
 * @root: heap tree want to search.
 * @parentp: pointer used to modify the parent node pointer.
 * @node: new node to insert.
 */
struct heap_node **
heap_parent(struct heap_root *root, struct heap_node **parentp, struct heap_node *node)
{
    unsigned int depth = flsuf(root->count + 1);
    struct heap_node **link;

    link = &root->node;
    if (unlikely(!*link)) {
        *parentp = NULL;
        return link;
    }

    while (depth--) {
        *parentp = *link;
        if ((root->count + 1) & BIT(depth))
            link = &(*link)->right;
        else
            link = &(*link)->left;
    };

    return link;
}
EXPORT_SYMBOL(heap_parent);

/**
 * heap_find - find @index in tree @root.
 * @root: heap tree want to search.
 * @index: index of node.
 */
struct heap_node *heap_find(struct heap_root *root, unsigned int index)
{
    unsigned int depth = flsuf(root->count);
    struct heap_node *node = root->node;

    while (node && depth--) {
        if ((root->count) & BIT(depth))
            node = node->right;
        else
            node = node->left;
    };

    return node;
}
EXPORT_SYMBOL(heap_find);

TITER_BASE_DEFINE(, heap, struct heap_node, left, right)
TITER_LEVELORDER_DEFINE(, heap_level, heap, struct heap_root, node, struct heap_node, left, right)

EXPORT_SYMBOL(heap_left_far);
EXPORT_SYMBOL(heap_right_far);
EXPORT_SYMBOL(heap_left_deep);
EXPORT_SYMBOL(heap_right_deep);
EXPORT_SYMBOL(heap_level_first);
EXPORT_SYMBOL(heap_level_next);
