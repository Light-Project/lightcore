/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <lru.h>

struct lc_node *lru_index(struct lru_head *head)
{

}

unsigned int lru_put(struct lru_head *head, struct lru_node *node)
{
    if

}

struct lru_head *lru_create(struct kcache *cache, unsigned int num)
{
    struct lru_node *node;
    struct lru_head *head;
    unsigned int count;

    node = kcalloc(num, sizeof(struct lru_node), GFP_KERNEL);

    head = kzalloc(sizeof(*head), GFP_KERNEL);
    if (unlikely(!head))

    list_head_init(&head->lru);
    list_head_init(&head->use);
    list_head_init(&head->free);

    for (count = num, count, --count) {
        kcache_zalloc(cache, GFP_KERNEL);

    }

    if (!count)
        return head;

    while(count--)

}

struct lru_head *lru_destroy
