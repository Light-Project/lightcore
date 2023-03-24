/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SEGTREE_
#define _SEGTREE_

#include <rbtree.h>

struct segtree_node {
    struct rb_node node;
    unsigned long start, end;
    unsigned long subtree;
};

/**
 * segtree_entry - get the struct for this entry.
 * @ptr: the &struct segtree_node pointer.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the segtree_node within the struct.
 */
#define	segtree_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * segtree_entry_safe - get the struct for this entry or null.
 * @ptr: the &struct segtree_node pointer.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the segtree_node within the struct.
 */
#define segtree_entry_safe(ptr, type, member) \
    container_of_safe(ptr, type, member)

extern void segtree_insert(struct rb_root_cached *root, struct segtree_node *node);
extern void segtree_delete(struct rb_root_cached *root, struct segtree_node *node);
extern struct segtree_node *segtree_search(struct segtree_node *node, unsigned long start, unsigned long end);
extern struct segtree_node *segtree_first(struct rb_root_cached *root, unsigned long start, unsigned long end);
extern struct segtree_node *segtree_next(struct segtree_node *node, unsigned long start, unsigned long end);

/**
 * segtree_first_entry - get the first element from a segtree.
 * @ptr: the rbtree root to take the element from.
 * @start: start endpoint of segtree element.
 * @end: end endpoint of segtree element.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the rb_node within the struct.
 */
#define segtree_first_entry(root, start, end, type, member) \
    segtree_entry_safe(segtree_first(root, start, end), type, member)

/**
 * segtree_next_entry - get the next element in segtree.
 * @pos: the type * to cursor.
 * @start: start endpoint of segtree element.
 * @end: end endpoint of segtree element.
 * @member: the name of the rb_node within the struct.
 */
#define segtree_next_entry(pos, start, end, member) \
    segtree_entry_safe(segtree_next(&(pos)->member, start, end), typeof(*(pos)), member)

/**
 * segtree_for_each - iterate over a segtree.
 * @pos: the &struct segtree_node to use as a loop cursor.
 * @start: start endpoint of segtree element.
 * @end: end endpoint of segtree element.
 * @root: the root for your segtree.
 */
#define segtree_for_each(pos, start, end, root) \
    for (pos = segtree_first(root, start, end); pos; \
         pos = segtree_next(pos, start, end))

/**
 * segtree_for_each_form - iterate over a segtree from the current point.
 * @pos: the &struct rb_node to use as a loop cursor.
 * @start: start endpoint of segtree element.
 * @end: end endpoint of segtree element.
 */
#define segtree_for_each_form(pos, start, end) \
    for (; pos; pos = segtree_next(pos, start, end))

/**
 * segtree_for_each_continue - continue iteration over a segtree.
 * @pos: the &struct segtree_node to use as a loop cursor.
 * @start: start endpoint of segtree element.
 * @end: end endpoint of segtree element.
 */
#define segtree_for_each_continue(pos, start, end) \
    for ((void)(pos && (pos = segtree_next(pos, start, end))); \
         pos; pos = segtree_next(pos, start, end))

/**
 * segtree_for_each_entry - iterate over segtree of given type.
 * @pos: the type * to use as a loop cursor.
 * @start: start endpoint of segtree element.
 * @end: end endpoint of segtree element.
 * @root: the root for your segtree.
 * @member: the name of the rb_node within the struct.
 */
#define segtree_for_each_entry(pos, start, end, root, member) \
    for (pos = segtree_first_entry(root, start, end, typeof(*(pos)), member); \
         pos; pos = segtree_next_entry(pos, start, end, member))

/**
 * segtree_for_each_entry_from - iterate over segtree of given type from the current point.
 * @pos: the type * to use as a loop cursor.
 * @start: start endpoint of segtree element.
 * @end: end endpoint of segtree element.
 * @member: the name of the rb_node within the struct.
 */
#define segtree_for_each_entry_from(pos, start, end, member) \
    for (; pos; pos = segtree_next_entry(pos, start, end, member))

/**
 * segtree_for_each_entry_continue - continue iteration over segtree of given type.
 * @pos: the type * to use as a loop cursor.
 * @start: start endpoint of segtree element.
 * @end: end endpoint of segtree element.
 * @member: the name of the rb_node within the struct.
 */
#define segtree_for_each_entry_continue(pos, start, end, member) \
    for ((void)(pos && (pos = segtree_next_entry(pos, start, end, member))); \
         pos; pos = segtree_next_entry(pos, start, end, member))

#define SEGTREE_DEFINE(STSTATIC, STNAME, STSTRUCT, STRB, STTYPE, STSUBTREE, STSTART, STEND)     \
RB_DECLARE_CALLBACKS_MAX(static, STNAME##_callbacks, STSTRUCT, STRB, STTYPE, STSUBTREE, STEND); \
STSTATIC void STNAME##_insert(struct rb_root_cached *cached, STSTRUCT *node)                    \
{                                                                                               \
    struct rb_node **link = &cached->root.node;                                                 \
    STSTRUCT *parent = NULL;                                                                    \
    STTYPE start, end;                                                                          \
    bool leftmost = true;                                                                       \
                                                                                                \
    start = STSTART(node);                                                                      \
    end = STEND(node);                                                                          \
                                                                                                \
    while (*link) {                                                                             \
        parent = rb_entry(*link, STSTRUCT, STRB);                                               \
        if (parent->STSUBTREE < end)                                                            \
            parent->STSUBTREE = end;                                                            \
        if (start < STSTART(parent))                                                            \
            link = &parent->STRB.left;                                                          \
        else {                                                                                  \
            link = &parent->STRB.right;                                                         \
            leftmost = false;                                                                   \
        }                                                                                       \
    }                                                                                           \
                                                                                                \
    rb_cached_insert_node_augmented(cached, parent ? &parent->STRB : NULL,                      \
                                    link, &node->STRB, leftmost, &STNAME##_callbacks);          \
    node->STSUBTREE = end;                                                                      \
}                                                                                               \
                                                                                                \
STSTATIC void STNAME##_delete(struct rb_root_cached *cached, STSTRUCT *node)                    \
{                                                                                               \
    rb_cached_delete_augmented(cached, &node->STRB, &STNAME##_callbacks);                       \
}                                                                                               \
                                                                                                \
STSTATIC STSTRUCT *STNAME##_search(STSTRUCT *node, STTYPE start, STTYPE end)                    \
{                                                                                               \
    STSTRUCT *walk;                                                                             \
                                                                                                \
    for (;;) {                                                                                  \
        if (node->STRB.left) {                                                                  \
            walk = rb_entry(node->STRB.left, STSTRUCT, STRB);                                   \
            if (walk->STSUBTREE >= start) {                                                     \
                node = walk;                                                                    \
                continue;                                                                       \
            }                                                                                   \
        }                                                                                       \
        if (STSTART(node) <= end) {                                                             \
            if (start <= STEND(node))                                                           \
                return node;                                                                    \
            else if (node->STRB.right) {                                                        \
                node = rb_entry(node->STRB.right, STSTRUCT, STRB);                              \
                if (node->STSUBTREE >= start)                                                   \
                    continue;                                                                   \
            }                                                                                   \
        }                                                                                       \
        return NULL;                                                                            \
    }                                                                                           \
}                                                                                               \
                                                                                                \
STSTATIC STSTRUCT *STNAME##_first(struct rb_root_cached *cached, STTYPE start, STTYPE end)      \
{                                                                                               \
    STSTRUCT *node, *leftmost;                                                                  \
                                                                                                \
    if (!(node = rb_entry_safe(cached->root.node, STSTRUCT, STRB)))                             \
        return NULL;                                                                            \
                                                                                                \
    if (node->STSUBTREE < start)                                                                \
        return NULL;                                                                            \
                                                                                                \
    leftmost = rb_cached_first_entry(cached, STSTRUCT, STRB);                                   \
    if (STSTART(leftmost) > end)                                                                \
        return NULL;                                                                            \
                                                                                                \
    return STNAME##_search(node, start, end);                                                   \
}                                                                                               \
                                                                                                \
STSTATIC STSTRUCT *STNAME##_next(STSTRUCT *node, STTYPE start, STTYPE end)                      \
{                                                                                               \
    struct rb_node *prev, *walk = node->STRB.right;                                             \
    STSTRUCT *right;                                                                            \
                                                                                                \
    for (;;) {                                                                                  \
        if ((right = rb_entry_safe(walk, STSTRUCT, STRB)) && right->STSUBTREE >= start)         \
            return STNAME##_search(right, start, end);                                          \
                                                                                                \
        do {                                                                                    \
            walk = node->STRB.parent;                                                           \
            if (!walk)                                                                          \
                return NULL;                                                                    \
            prev = &node->STRB;                                                                 \
            node = rb_entry(walk, STSTRUCT, STRB);                                              \
            walk = node->STRB.right;                                                            \
        } while (walk == prev);                                                                 \
                                                                                                \
        if (end < STSTART(node))                                                                \
            return NULL;                                                                        \
        else if (start <= STEND(node))                                                          \
            return node;                                                                        \
    }                                                                                           \
}

#endif /* _SEGTREE_ */
