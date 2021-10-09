/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LRU_H_
#define _LRU_H_

#include <list.h>

struct lru_head {
    struct list_head lru;
    struct list_head use;
    struct list_head free;
};

struct lru_node {
    struct list_head list;
};

struct lru_head *lru_create();
void lru_create(struct lru_head *);

#endif  /* _LRU_H_ */
