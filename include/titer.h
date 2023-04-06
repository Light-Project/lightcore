/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TITER_H_
#define _TITER_H_

#include <stddef.h>
#include <bitops.h>

#define TITER_BASE_DEFINE(TISTATIC, TINAME, TISTRUCT, TILEFT, TIRIGHT)      \
TISTATIC TISTRUCT *TINAME##_left_far(const TISTRUCT *node)                  \
{                                                                           \
    /* Go left as we can */                                                 \
    while (node->TILEFT)                                                    \
        node = node->TILEFT;                                                \
                                                                            \
    return (TISTRUCT *)node;                                                \
}                                                                           \
                                                                            \
TISTATIC TISTRUCT *TINAME##_right_far(const TISTRUCT *node)                 \
{                                                                           \
    /* Go right as we can */                                                \
    while (node->TIRIGHT)                                                   \
        node = node->TIRIGHT;                                               \
                                                                            \
    return (TISTRUCT *)node;                                                \
}                                                                           \
                                                                            \
TISTATIC TISTRUCT *TINAME##_left_deep(const TISTRUCT *node)                 \
{                                                                           \
    /* Go left deep as we can */                                            \
    while (node) {                                                          \
        if (node->TILEFT)                                                   \
            node = node->TILEFT;                                            \
        else if (node->TIRIGHT)                                             \
            node = node->TIRIGHT;                                           \
        else                                                                \
            return (TISTRUCT *)node;                                        \
    }                                                                       \
                                                                            \
    return NULL;                                                            \
}                                                                           \
                                                                            \
TISTATIC TISTRUCT *TINAME##_right_deep(const TISTRUCT *node)                \
{                                                                           \
    /* Go right deep as we can */                                           \
    while (node) {                                                          \
        if (node->TIRIGHT)                                                  \
            node = node->TIRIGHT;                                           \
        else if (node->TILEFT)                                              \
            node = node->TILEFT;                                            \
        else                                                                \
            return (TISTRUCT *)node;                                        \
    }                                                                       \
                                                                            \
    return NULL;                                                            \
}

#define TITER_INORDER_DEFINE(TISTATIC, TINAME, BNAME, TIROOT, TINODE,       \
                             TISTRUCT, TIPARENT, TILEFT, TIRIGHT)           \
TISTATIC TISTRUCT *TINAME##_first(const TIROOT *root)                       \
{                                                                           \
    TISTRUCT *node = root->TINODE;                                          \
                                                                            \
    if (!root || !node)                                                     \
        return NULL;                                                        \
                                                                            \
    /* Get the leftmost node */                                             \
    node = BNAME##_left_far(node);                                          \
    return node;                                                            \
}                                                                           \
                                                                            \
TISTATIC TISTRUCT *TINAME##_last(const TIROOT *root)                        \
{                                                                           \
    TISTRUCT *node = root->TINODE;                                          \
                                                                            \
    if (!root || !node)                                                     \
        return NULL;                                                        \
                                                                            \
    /* Get the rightmost node */                                            \
    node = BNAME##_right_far(node);                                         \
    return node;                                                            \
}                                                                           \
                                                                            \
TISTATIC TISTRUCT *TINAME##_prev(const TISTRUCT *node)                      \
{                                                                           \
    TISTRUCT *parent;                                                       \
                                                                            \
    if (!node)                                                              \
        return NULL;                                                        \
                                                                            \
    /*                                                                      \
     * If there is a left-hand node, go down                                \
     * and then as far right as possible.                                   \
     */                                                                     \
    if (node->TILEFT) {                                                     \
        node = node->TILEFT;                                                \
        return BNAME##_right_far(node);                                     \
    }                                                                       \
                                                                            \
    /*                                                                      \
     * No left-hand children. Go up till we find an ancestor                \
     * which is a right-hand child of its parent.                           \
     */                                                                     \
    while ((parent = node->TIPARENT) && node != parent->TIRIGHT)            \
        node = parent;                                                      \
                                                                            \
    return parent;                                                          \
}                                                                           \
                                                                            \
TISTATIC TISTRUCT *TINAME##_next(const TISTRUCT *node)                      \
{                                                                           \
    TISTRUCT *parent;                                                       \
                                                                            \
    if (!node)                                                              \
        return NULL;                                                        \
                                                                            \
    /*                                                                      \
     * If there is a right-hand node, go down                               \
     * and then as far left as possible.                                    \
     */                                                                     \
    if (node->TIRIGHT) {                                                    \
        node = node->TIRIGHT;                                               \
        return BNAME##_left_far(node);                                      \
    }                                                                       \
                                                                            \
    /*                                                                      \
     * No right-hand children. Go up till we find an ancestor               \
     * which is a left-hand child of its parent.                            \
     */                                                                     \
    while ((parent = node->TIPARENT) && node != parent->TILEFT)             \
        node = parent;                                                      \
                                                                            \
    return parent;                                                          \
}

#define TITER_PREORDER_DEFINE(TISTATIC, TINAME, BNAME, TIROOT, TINODE,      \
                              TISTRUCT, TIPARENT, TILEFT, TIRIGHT)          \
TISTATIC TISTRUCT *TINAME##_first(const TIROOT *root)                       \
{                                                                           \
    /* Get the root node */                                                 \
    return root->TINODE;                                                    \
}                                                                           \
                                                                            \
TISTATIC TISTRUCT *TINAME##_next(const TISTRUCT *node)                      \
{                                                                           \
    TISTRUCT *parent;                                                       \
                                                                            \
    if (!node)                                                              \
        return NULL;                                                        \
                                                                            \
    /**                                                                     \
     * If there are left and right child nodes,                             \
     * then we iterate directly.                                            \
     */                                                                     \
    if (node->TILEFT)                                                       \
        return node->TILEFT;                                                \
                                                                            \
    if (node->TIRIGHT)                                                      \
        return node->TIRIGHT;                                               \
                                                                            \
    /**                                                                     \
     * If we have no children, Go up till we find an ancestor               \
     * which have a another right-hand child.                               \
     */                                                                     \
    while ((parent = node->TIPARENT) &&                                     \
           (!parent->TIRIGHT || node == parent->TIRIGHT))                   \
        node = parent;                                                      \
                                                                            \
    return parent ? parent->TIRIGHT : NULL;                                 \
}

#define TITER_POSTORDER_DEFINE(TISTATIC, TINAME, BNAME, TIROOT, TINODE,     \
                               TISTRUCT, TIPARENT, TILEFT, TIRIGHT)         \
TISTATIC TISTRUCT *TINAME##_first(const TIROOT *root)                       \
{                                                                           \
    TISTRUCT *node = root->TINODE;                                          \
                                                                            \
    if (!root || !node)                                                     \
        return NULL;                                                        \
                                                                            \
    /* Get the left deepest node. */                                        \
    return BNAME##_left_deep(node);                                         \
}                                                                           \
                                                                            \
TISTATIC TISTRUCT *TINAME##_next(const TISTRUCT *node)                      \
{                                                                           \
    const TISTRUCT *parent;                                                 \
                                                                            \
    if (!node)                                                              \
        return NULL;                                                        \
    parent = node->TIPARENT;                                                \
                                                                            \
    /**                                                                     \
     * If we are the parent's left node, go to the parent's right           \
     * node then all the way down to the left.                              \
     * Otherwise we are the parent's right node, and the parent             \
     * should be next.                                                      \
     */                                                                     \
    if (parent && node == parent->TILEFT && parent->TIRIGHT)                \
        return BNAME##_left_deep(parent->TIRIGHT);                          \
    else                                                                    \
        return (TISTRUCT *)parent;                                          \
}

#define TITER_LEVELORDER_DEFINE(TISTATIC, TINAME, BNAME, TIROOT, TINODE,    \
                                TISTRUCT, TILEFT, TIRIGHT)                  \
TISTATIC TISTRUCT *                                                         \
TINAME##_first(const TIROOT *root, unsigned long *index)                    \
{                                                                           \
    /* Initialize counter and get root node */                              \
    *index = 0;                                                             \
    return root->TINODE;                                                    \
}                                                                           \
                                                                            \
TISTATIC TISTRUCT *                                                         \
TINAME##_next(const TIROOT *root, unsigned long *index)                     \
{                                                                           \
    unsigned int depth = flsuf(++*index + 1);                               \
    TISTRUCT *node = root->node;                                            \
                                                                            \
    while (node && depth--) {                                               \
        if ((*index + 1) & BIT(depth))                                      \
            node = node->TIRIGHT;                                           \
        else                                                                \
            node = node->TILEFT;                                            \
    };                                                                      \
                                                                            \
    return node;                                                            \
}

#endif /* _TITER_H_ */
