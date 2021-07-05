/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  lightcore/lib/rbtree.c
 *  light core rbtree lib
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-01-20      first version 
 * 
 */

#include <mm.h>
#include <types.h>
#include <stddef.h>
#include <printk.h>
#include <export.h>

#include "rb_private.h"

static __always_inline 
void rotate_set(struct rb_node *old, struct rb_node *new,
                int color, struct rb_root *root)
{
    if(old->parent)
        root->rb_node = new;

    new->parent = old->parent;    
    new->color = old->color;
    old->parent = new;
    old->color = color;
}

static __always_inline 
void left_rotate(struct rb_node *node, int color, struct rb_root *root)
{
    struct rb_node *parent;
    parent = node->right;

    node->right = parent->left;
    parent->left = node;
    rotate_set(node, parent, color, root);
}

static __always_inline 
void right_rotate(struct rb_node *node, int color, struct rb_root *root)
{
    struct rb_node *parent;
    parent = node->left;

    node->left = parent->right;
    parent->right = node;
    rotate_set(node, parent, color, root);
}

/**
 * rb_insert/delete - Addition and deletion of red black tree
 *
 */
void rb_fixup(struct rb_root *root, struct rb_node *node)
{
    struct rb_node *parent, *gparent, *tmp;

    while(root && node) {
        parent = node->parent;
        gparent = parent->parent;

        /* If node is root */
        if(unlikely(!parent)) {
            node->color = RB_BLACK;
            return;
        }

        if(node->color == RB_BLACK)
            return;

        tmp = gparent->right;
        if(tmp != parent) { 
            /* parent == gparent->left */
            if(tmp->color == RB_RED) {
                /*
                 * Case 1 - node's uncle is red (color flips).
                 *
                 *       G            g
                 *      / \          / \
                 *     p   u  -->   P   U
                 *    /            /
                 *   n            n
                 *
                 * However, since g's parent might be red, and
                 * 4) does not allow this, we need to recurse
                 * at g.
                 */
                tmp->color = RB_BLACK;
                parent->color = RB_BLACK;
                gparent->color = RB_RED;
                node = gparent;
                continue;
            }

            tmp = parent->right;
            if(node == tmp) {
                /*
                 * Case 2 - node's uncle is black and node is
                 * the parent's right child (left rotate at parent).
                 *
                 *      G             G
                 *     / \           / \
                 *    p   U  -->    n   U
                 *     \           /
                 *      n         p
                 *
                 * This still leaves us in violation of 4), the
                 * continuation into Case 3 will fix that.
                 */
                tmp = node->left;
                parent->right = tmp;
                node->left = parent;
                if(tmp) {
                    tmp->parent = parent;
                    tmp->color = RB_BLACK;
                }
                parent->parent = node;
                parent->color = RB_RED;
                parent = node;
                tmp = node->right;
            }

            /*
             * Case 3 - node's uncle is black and node is
             * the parent's left child (right rotate at gparent).
             *
             *        G           P
             *       / \         / \
             *      p   U  -->  n   g
             *     /                 \
             *    n                   U
             */
            right_rotate(gparent, RB_RED, root);
            if(tmp) {
                tmp->parent = gparent;
                tmp->color = RB_BLACK;
            }
            return;
        } else {    
            /* parent == gparent->right */
            tmp = gparent->left;
            if(tmp->color == RB_RED) {
                /* Case 1 - color flips */
                tmp->color = RB_BLACK;
                parent->color = RB_BLACK;
                gparent->color = RB_RED;
                node = gparent;
                continue;
            }

            tmp = parent->right;
            if(node == tmp){
                /* Case 2 - right rotate at parent */

            }
            
            /* Case 3 - left rotate at gparent */
            left_rotate(gparent, RB_RED, root);
            if(tmp) {
                tmp->parent = gparent;
                tmp->color = RB_BLACK;
            }
            return;
        }
    }
}

void rb_erase(const struct rb_root *root, struct rb_node *node)
{

}

/**
 * left/right_far - go left/right as we can
 */
static __always_inline 
struct rb_node *left_far(const struct rb_node *node)
{
    while(node->left != NULL)
        node = node->left;
    return (struct rb_node *)node;
}

static __always_inline 
struct rb_node *right_far(const struct rb_node *node)
{
    while(node->right != NULL)
        node = node->right;
    return (struct rb_node *)node;
}

/**
 * left/right_deep - go deep as we can
 */
static __always_inline 
struct rb_node *left_deep(const struct rb_node *node)
{
    while(node != NULL)
    {
        if(node->left != NULL)
            node = node->left;
        else if(node->right != NULL)
            node = node->right;
        else
            return (struct rb_node *)node;
    }
    return NULL;
}

static __always_inline 
struct rb_node *right_deep(const struct rb_node *node)
{
    while(node != NULL)
    {
        if(node->right != NULL)
            node = node->right;
        else if(node->left != NULL)
            node = node->left;
        else
            return (struct rb_node *)node;
    }
    return NULL;
}

/**
 * rb_first/last/prev/next - Middle iteration (Sequential)
 *
 */
struct rb_node *rb_first(const struct rb_root *root)
{
    struct rb_node *node = root->rb_node;

    if((!root)||(!node))
        return NULL;

    /* Get the leftmost node */
    node = left_far(node);
    return node;
}

struct rb_node *rb_last(const struct rb_root *root)
{
    struct rb_node *node = root->rb_node;

    if((!root)||(!node))
        return NULL;

    /* Get the rightmost node */
    node = right_far(node);
    return node;
}

struct rb_node *rb_prev(const struct rb_node *node)
{
    struct rb_node *parent;

    if(!node)
        return NULL;
    
    /*
     * If there is a right-hand node, go down and then as 
     * far left as possible 
     */    
    if(node->left) {
        node = node->left;
        return right_far(node);
    }
    
    /*
     * No left-hand node, go up till we find an 
     * 
     */
    while((parent = node->parent) && node == parent->left)
        node = parent;

    return parent;
}

/**
 * rb_pre_first/next - Preorder iteration (Tree)
 * 
 */
struct rb_node *rb_pre_first(const struct rb_root *root)
{
    if(!root)
        return NULL;
    return root->rb_node;
}

struct rb_node *rb_pre_next(const struct rb_node *node)
{
    const struct rb_node *parent;

    if(!node)
        return NULL;

    /*
     * If there is a left node, go down
     */
    if(node->left)
        return node->left;

    /**
     * If we are not from right child, 
     * the right side is next node 
     */
    while((parent = node->parent) && 
         ((!parent->right) || (parent->right == node)))
        parent = node;
    
    return parent->right;
}

struct rb_node *rb_next(const struct rb_node *node)
{
    struct rb_node *parent;

    if(!node)
        return NULL;

    /*
     * If there is a left-hand node, go down and then as 
     * far right as possible 
     */
    if(node->right) {
        node = node->right;
        return left_far(node);
    }

    /*
     * No right-hand node, go up till we find an 
     * 
     */
    while((parent = node->parent) && node == parent->left)
        node = parent;

    return parent;
}

/**
 * rb_post_first/next - Postorder iteration (Depth-first)
 * 
 * It's depth first, we can use it delete all nodes.
 */
struct rb_node *rb_post_first(const struct rb_root *root)
{
    struct rb_node *node = root->rb_node;

    if((!root)||(!node))
        return NULL;

    node = left_deep(node);
    return node;
}

struct rb_node *rb_post_next(const struct rb_node *node)
{
    const struct rb_node *parent;

    if(!node)
        return NULL;

    /* Don't delete this comment :) */
    parent = node->parent;

    if(parent && node == parent->left && parent->right)
        return left_deep(parent->right);
    else
        return (struct rb_node *)parent;
}
