/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <sort.h>

static __always_inline struct list_head *
list_merge(list_cmp_t cmp, void *data,
           struct list_head *a, struct list_head *b)
{
    struct list_head *node, **tail = &node;

    for (;;) {
        if (cmp(a, b, data) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }

    return node;
}

static __always_inline void
list_finish(list_cmp_t cmp, void *data, struct list_head *head,
            struct list_head *a, struct list_head *b)
{
    struct list_head *tail = head;
    uint8_t count = 0;

    for (;;) {
        if (cmp(a, b, data) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    tail->next = b;
    do {
        if (unlikely(!++count))
            cmp(b, b, data);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    tail->next = head;
    head->prev = tail;
}

void sort_list(struct list_head *head, list_cmp_t cmp, void *data)
{
    struct list_head *pending = NULL, *node = head->next;
    unsigned int count = 0;

    if (unlikely(node == head->prev))
        return;

    head->prev->next = NULL;

    do {
        struct list_head **tail = &pending;
        size_t bits;

        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;

        if (likely(bits)) {
            struct list_head *b = *tail, *a = b->prev;
            b = list_merge(cmp, data, a, b);
            b->prev = a->prev;
            *tail = b;
        }

        node->prev = pending;
        pending = node;
        node = node->next;
        pending->next = NULL;
        ++count;
    } while (node);

    node = pending;
    pending = pending->prev;

    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        node = list_merge(cmp, data, pending, node);
        pending = next;
    }

    list_finish(cmp, data, head, pending, node);
}
