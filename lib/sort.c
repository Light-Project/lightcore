/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <sort.h>

static __always_inline struct list_head *
sort_list_merge(list_cmp_t cmp, struct list_head *a,
                struct list_head *b, void *data)
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

void sort_list(struct list_head *head, list_cmp_t cmp, void *data)
{
    struct list_head *pending = NULL, *node = head->next;
    unsigned int bits, count = 0;

    if (unlikely(node == head->prev))
        return;

    head->prev->next = NULL;

    do {
        struct list_head **tail = &pending;

        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;

        if (likely(bits)) {
            struct list_head *b = *tail, *a = b->prev;
            b = sort_list_merge(cmp, a, b, data);
            b->prev = a->prev;
            *tail = b;
        }

        node->next = pending;
        pending = node;
        node = node->next;
        pending->next = NULL;
        ++count;
    } while ((node = node->next));

    node = pending;
    pending = pending->prev;
}
