/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <mm.h>
#include <types.h>
#include <stddef.h>
#include <printk.h>
#include <rbtree.h>
#include <export.h>

static inline void
rotate_set(struct rb_node *old, struct rb_node *new,
           int color, struct rb_root *root)
{
    if (unlikely(old->parent))
        root->rb_node = new;

    new->parent = old->parent;    
    new->color = old->color;
    old->parent = new;
    old->color = color;
}

static inline void
left_rotate(struct rb_node *node, int color, struct rb_root *root)
{
    struct rb_node *parent;
    parent = node->right;

    node->right = parent->left;
    parent->left = node;
    rotate_set(node, parent, color, root);
}

static inline void
right_rotate(struct rb_node *node, int color, struct rb_root *root)
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

        /* If node is root */
        if (unlikely(!parent)) {
            node->color = RB_BLACK;
            return;
        }

        /*
         * If there is a black parent, we are done.
         */
        if (parent->color == RB_BLACK)
            return;

        gparent = parent->parent;
        tmp = gparent->right;

        if (tmp != parent) { 
            /* parent == gparent->left */
            if (tmp && tmp->color == RB_RED) {
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
                gparent->color = RB_RED;
                parent->color = tmp->color = RB_BLACK;
                node = gparent;
                parent = node->parent;
                continue;
            }

            tmp = parent->right;
            if (node == tmp) {
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
                if (tmp)
                    tmp->color = RB_BLACK;
                left_rotate(parent, RB_RED, root);
                gparent->left = node;
                parent = node;
                tmp = parent->right;
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
            if (tmp)
                tmp->color = RB_BLACK;
            right_rotate(gparent, RB_RED, root);
            return;
        } else {    
            /* parent == gparent->right */
            tmp = gparent->left;
            if (tmp && tmp->color == RB_RED) {
                /* Case 1 - color flips */
                gparent->color = RB_RED;
                parent->color = tmp->color = RB_BLACK;
                node = gparent;
                parent = node->parent;
                continue;
            }

            tmp = parent->left;
            if (node == tmp){
                /* Case 2 - right rotate at parent */
                tmp = node->right;
                if (tmp)
                    tmp->color = RB_BLACK;
                right_rotate(parent, RB_RED, root);
                gparent->right = node;
                parent = node;
                tmp = parent->left;
            }
            
            /* Case 3 - left rotate at gparent */
            if(tmp)
                tmp->color = RB_BLACK;
            left_rotate(gparent, RB_RED, root);
            return;
        }
    }
}

void rb_del(const struct rb_root *root, struct rb_node *node)
{


}

/**
 * left/right_far - go left/right as we can
 */
static __always_inline 
struct rb_node *left_far(const struct rb_node *node)
{
    while (node->left)
        node = node->left;
    return (struct rb_node *)node;
}

static __always_inline 
struct rb_node *right_far(const struct rb_node *node)
{
    while (node->right)
        node = node->right;
    return (struct rb_node *)node;
}

/**
 * left/right_deep - go deep as we can
 */
static __always_inline 
struct rb_node *left_deep(const struct rb_node *node)
{
    while (node) {
        if (node->left != NULL)
            node = node->left;
        else if (node->right != NULL)
            node = node->right;
        else
            return (struct rb_node *)node;
    }
    return NULL;
}

static __always_inline 
struct rb_node *right_deep(const struct rb_node *node)
{
    while (node) {
        if (node->right != NULL)
            node = node->right;
        else if (node->left != NULL)
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

    if (!root || !node)
        return NULL;

    /* Get the leftmost node */
    node = left_far(node);
    return node;
}

struct rb_node *rb_last(const struct rb_root *root)
{
    struct rb_node *node = root->rb_node;

    if(!root || !node)
        return NULL;

    /* Get the rightmost node */
    node = right_far(node);
    return node;
}

struct rb_node *rb_prev(const struct rb_node *node)
{
    struct rb_node *parent;

    if (!node)
        return NULL;
    
    /*
     * If there is a right-hand node, go down and then as 
     * far left as possible 
     */    
    if (node->left) {
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
    if (!root)
        return NULL;
    return root->rb_node;
}

struct rb_node *rb_pre_next(const struct rb_node *node)
{
    const struct rb_node *parent;

    if (!node)
        return NULL;

    /*
     * If there is a left node, go down
     */
    if (node->left)
        return node->left;

    /**
     * If we are not from right child, 
     * the right side is next node 
     */
    while((parent = node->parent) && ((!parent->right) || 
          (parent->right == node)))
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
    if (node->right) {
        node = node->right;
        return left_far(node);
    }

    /*
     * No right-hand node, go up till we find an 
     * 
     */
    while ((parent = node->parent) && node == parent->left)
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

    if (!root || !node)
        return NULL;

    node = left_deep(node);
    return node;
}

struct rb_node *rb_post_next(const struct rb_node *node)
{
    const struct rb_node *parent;

    if (!node)
        return NULL;

    parent = node->parent;

    if (parent && node == parent->left && parent->right)
        return left_deep(parent->right);
    else
        return (struct rb_node *)parent;
}
