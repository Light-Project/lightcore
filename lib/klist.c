/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <klist.h>
#include <sched.h>
#include <crash.h>
#include <export.h>

struct klist_wait {
    struct list_head list;
    struct sched_task *task;
    struct klist_node *node;
    bool woken;
};

static __always_inline void
lock_add_next(struct klist_head *head, struct list_head *node, struct list_head *new)
{
    spin_lock(&head->lock);
    list_add(node, new);
    spin_unlock(&head->lock);
}

static __always_inline void
lock_add_prev(struct klist_head *head, struct list_head *node, struct list_head *new)
{
    spin_lock(&head->lock);
    list_add_prev(node, new);
    spin_unlock(&head->lock);
}

static __always_inline void
klist_init(struct klist_head *head, struct klist_node *node)
{
    list_head_init(&node->list);
    kref_init(&node->kref);
    node->head = head;
}

static void klist_release(struct kref *kref)
{
    struct klist_node *node = container_of(kref, struct klist_node, kref);
    struct klist_head *head = node->head;
    struct klist_wait *walk, *tmp;

    WARN_ON(!node->dead);
    list_del(&node->list);

    spin_lock(&head->wlock);
    list_for_each_entry_safe(walk, tmp, &head->wait, list) {
        if (walk->node != node)
            continue;

        walk->woken = true;
        smp_mb();

        sched_wake_up(walk->task);
        list_del(&walk->list);
    }
    spin_unlock(&head->wlock);
}

static inline bool klist_kref_put(struct klist_node *node)
{
    return kref_put(&node->kref, klist_release);
}

static void klist_put(struct klist_node *node, bool kill)
{
    void (*put)(struct klist_node *) = node->head->put;
    struct klist_head *head = node->head;

    spin_lock(&head->lock);

    if (kill) {
        BUG_ON(node->dead);
        node->dead = true;
    }

    if (!klist_kref_put(node))
        put = NULL;

    spin_unlock(&head->lock);

    if (put)
        put(node);
}

/**
 * klist_add_head - insert a new node on list head
 * @head: inserted head
 * @new: node to insert head
 */
void klist_add_head(struct klist_head *head, struct klist_node *new)
{
    klist_init(head, new);
    lock_add_next(head, &head->node, &new->list);
}
EXPORT_SYMBOL(klist_add_head);

/**
 * klist_add_tail - insert a new node on list tail
 * @head: inserted head
 * @new: node to insert tail
 */
void klist_add_tail(struct klist_head *head, struct klist_node *new)
{
    klist_init(head, new);
    lock_add_next(head, &head->node, &new->list);
}
EXPORT_SYMBOL(klist_add_tail);

/**
 * klist_add_next - insert a new node next old node
 * @node: inserted node
 * @new: node to insert next
 */
void klist_add_next(struct klist_node *node, struct klist_node *new)
{
    struct klist_head *head = node->head;
    klist_init(head, new);
    lock_add_next(head, &node->list, &new->list);
}
EXPORT_SYMBOL(klist_add_next);

/**
 * klist_add_prev - insert a new node behind old node
 * @node: inserted node
 * @new: node to insert behind
 */
void klist_add_prev(struct klist_node *node, struct klist_node *new)
{
    struct klist_head *head = node->head;
    klist_init(head, new);
    lock_add_prev(head, &node->list, &new->list);
}
EXPORT_SYMBOL(klist_add_prev);

/**
 * klist_del - delete a klist node
 * @node: node to delete
 */
void klist_del(struct klist_node *node)
{
	klist_put(node, true);
}
EXPORT_SYMBOL(klist_del);

void klist_remove(struct klist_node *node)
{
    struct klist_head *head = node->head;
    struct klist_wait wait = {};

	wait.node = node;
	wait.task = current;

    spin_lock(&head->wlock);
    list_add(&head->wait, &wait.list);
    spin_unlock(&head->wlock);

    while (!wait.woken) {
        current_set_state(SCHED_TASK_UNINTERRUPTIBLE);
        sched_resched();
    }
}

/**
 * klist_iter_next - get next form a klist iteration
 * @iter: klist iteration context
 */
struct klist_node *klist_iter_next(struct klist_iter *iter)
{
    void (*put)(struct klist_node *) = iter->head->put;
    struct klist_node *node, *last = iter->curr;
    struct klist_head *head = iter->head;
    irqflags_t irqflags;

    spin_lock_irqsave(&head->lock, &irqflags);

    iter->curr = NULL;

    /* find the next node */
    if (!last)
        node = list_to_klist(head->node.next);
    else {
		node = list_to_klist(last->list.next);
        if (!klist_kref_put(node))
            put = NULL;
    }

    /* searching next surviving node */
    while (node != list_to_klist(&head->node)) {
        if (likely(!node->dead)) {
			kref_get(&node->kref);
			iter->curr = node;
			break;
		}
        node = list_to_klist(node->list.next);
    }

    spin_unlock_irqrestore(&head->lock, &irqflags);

    if (put)
        put(node);

    return iter->curr;
}
EXPORT_SYMBOL(klist_iter_next);

/**
 * klist_iter_prev - get prev form a klist iteration
 * @iter: klist iteration context
 */
struct klist_node *klist_iter_prev(struct klist_iter *iter)
{
    void (*put)(struct klist_node *) = iter->head->put;
    struct klist_node *node, *last = iter->curr;
    struct klist_head *head = iter->head;
    irqflags_t irqflags;

    spin_lock_irqsave(&head->lock, &irqflags);

    iter->curr = NULL;

    /* find the prev node */
    if (!last)
        node = list_to_klist(head->node.prev);
    else {
		node = list_to_klist(last->list.prev);
        if (!klist_kref_put(node))
            put = NULL;
    }

    /* searching prev surviving node */
    while (node != list_to_klist(&head->node)) {
        if (likely(!node->dead)) {
			kref_get(&node->kref);
			iter->curr = node;
			break;
		}
        node = list_to_klist(node->list.prev);
    }

    spin_unlock_irqrestore(&head->lock, &irqflags);

    if (put)
        put(node);

    return iter->curr;
}
EXPORT_SYMBOL(klist_iter_prev);

/**
 * klist_iter_init_node - start a klist iteration form node
 * @iter: iteration to start
 * @head: klist head to iteration
 * @node: iteration start node
 */
void klist_iter_init_node(struct klist_iter *iter, struct klist_head *head, struct klist_node *node)
{
    iter->head = head;
    iter->curr = node;
}
EXPORT_SYMBOL(klist_iter_init_node);

/**
 * klist_iter_init - start a klist iteration
 * @iter: iteration to start
 * @head: klist head to iteration
 */
void klist_iter_init(struct klist_iter *iter, struct klist_head *head)
{
    iter->head = head;
    iter->curr = NULL;
}
EXPORT_SYMBOL(klist_iter_init);

/**
 * klist_iter_exit - finish a klist iteration
 * @iter: iteration to finish
 */
void klist_iter_exit(struct klist_iter *iter)
{
    if (!iter->curr)
        return;
    klist_put(iter->curr, false);
    iter->curr = NULL;
}
EXPORT_SYMBOL(klist_iter_exit);
