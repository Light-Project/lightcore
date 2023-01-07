/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KLIST_H_
#define _KLIST_H_

#include <kref.h>
#include <waitqueue.h>

struct klist_node {
    struct list_head list;
    struct klist_head *head;
    struct kref kref;
    bool dead;
};

#define list_to_klist(node) \
    container_of(node, struct klist_node, list)

struct klist_head {
    spinlock_t lock;
    struct list_head node;

    spinlock_t wlock;
    struct list_head wait;

    void (*get)(struct klist_node *node);
    void (*put)(struct klist_node *node);
};

struct klist_iter {
    struct klist_head *head;
    struct klist_node *curr;
};

extern void klist_add_head(struct klist_head *head, struct klist_node *new);
extern void klist_add_tail(struct klist_head *head, struct klist_node *new);
extern void klist_add_next(struct klist_node *node, struct klist_node *new);
extern void klist_add_prev(struct klist_node *node, struct klist_node *new);
extern void klist_del(struct klist_node *node);

extern void klist_iter_init_node(struct klist_iter *iter, struct klist_head *head, struct klist_node *node);
extern void klist_iter_init(struct klist_iter *iter, struct klist_head *head);
extern void klist_iter_exit(struct klist_iter *iter);
extern struct klist_node *klist_iter_next(struct klist_iter *iter);
extern struct klist_node *klist_iter_prev(struct klist_iter *iter);

#endif  /* _KLIST_H_ */
