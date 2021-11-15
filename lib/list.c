/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <list.h>

void list_head_init(struct list_head *list)
{
    list->prev = list;
    list->next = list;
}

void list_insert(struct list_head *prev,
                 struct list_head *next,
                 struct list_head *new)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

void list_add(struct list_head *head,
              struct list_head *new)
{
    list_insert(head, head->next, new);
}

/**
 * list_add_prev - add a new node after old node
 * @node: list head to add it next
 * @new: new entry to be added
 */
void list_add_prev(struct list_head *node,
                   struct list_head *new)
{
    list_insert(node->prev , node, new);
}

/**
 * list_add_next - add a new node next old node
 * @node: list head to add it next
 * @new: new entry to be added
 */
void list_add_next(struct list_head *node,
                   struct list_head *new)
{
    list_insert(node, node->next, new);
}

/**
 * list_del - in fact, it just connect next and prev node
 * @node: list head to add it next
 * @new: new entry to be added
 */
void list_del(struct list_head *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

/**
 * list_replace - Replace a linked list node with an external node
 * @node: list head to add it next
 * @new:  new entry to be added
 */
void list_replace(struct list_head *old,
                  struct list_head *new)
{
    new->prev = old->prev;
    new->next = old->next;
    new->prev->next = new;
    new->next->prev = new;
}

/**
 * list_move_front - Move the node to the tail of the list
 * @node: list head to add it next
 * @new: new entry to be added
 */
void list_move_front(struct list_head *head, struct list_head *node)
{
    list_del(node);
    list_add_next(head, node);
}

/**
 * list_move_tail - Move the node to the tail of the list
 * @node: list head to add it next
 * @new: new entry to be added
 */
void list_move_tail(struct list_head *head, struct list_head *node)
{
    list_del(node);
    list_add_prev(head, node);
}

/**
 * list_check_first - Check whether the node is a header
 * @node: list head to add it next
 * @new: new entry to be added
 */
bool list_check_first(struct list_head *head,
                      struct list_head *node)
{
    return node->prev == head;
}

/**
 * list_check_end - Check whether the node is a ending
 * @node: list head to add it next
 * @new: new entry to be added
 */
bool list_check_end(struct list_head *head,
                     struct list_head *node)
{
    return node->next == head;
}

/**
 * list_check_around - Check whether the node is a ending
 * @node: list head to add it next
 * @new: new entry to be added
 */
bool list_check_around(struct list_head *prev,
                     struct list_head *next)
{
    return prev->next == next;
}

/**
 * list_check_empty - add a new node next old node
 * @para head: list head to check
 */
bool list_check_empty(struct list_head *head)
{
    return head->next == head;
}

/**
 * list_add_next: add a new node next old node
 * @node: list head to add it next
 * @new: new entry to be added
 */
bool list_length(struct list_head *head)
{
    return 0;
}
