/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LRU_CACHE_H_
#define _LRU_CACHE_H_

#include <list.h>
#include <hlist.h>
#include <bitflags.h>

typedef struct lrc_node *(*lrc_alloc)(void *pdata);
typedef void (*lrc_free)(struct lrc_node *node, void *pdata);

enum lrc_obtain {
    __LRC_CHANGE            = 0,
    __LRC_UNCOMMITTED       = 1,
};

enum lrc_flags {
    __LRC_DIRTY             = 0,
    __LRC_STARVING          = 1,
};

#define LRC_CHANGE          BIT(__LRC_CHANGE)
#define LRC_UNCOMMITTED     BIT(__LRC_UNCOMMITTED)
#define LRC_DIRTY           BIT(__LRC_DIRTY)
#define LRC_STARVING        BIT(__LRC_STARVING)
#define LRC_FREE_TAG        UINT_MAX

/**
 * struct lrc_node - least recently used cache node.
 * @hash: for hash table lookup.
 * @list: lru list or free list.
 * @index: node index in cache head.
 */
struct lrc_node {
    struct hlist_node hash;
    struct list_head list;
    unsigned int index;

    unsigned int refcnt;
    unsigned int tag;
    bool uncommitted;
};

/**
 * struct lrc_head - least recently used cache head.
 * @lru: available node list headers.
 * @using: using node list.
 * @freed: released node list.
 * @changing: status transition node to be commit.
 * @max_pending: maximum number of simultaneous changes that can be pending.
 * @used: number of elements currently in use.
 * @pending: number of elements currently in transition.
 * @changed: time of state transitions.
 * @hits: time of cache hits.
 * @misses: time of cache misses.
 * @starve: time of cache starvation.
 */
struct lrc_head {
    struct list_head lru;
    struct list_head using;
    struct list_head freed;
    struct list_head changing;

    struct hlist_head *taghash;
    struct lrc_node **nodes;

    /* const settings */
    unsigned int nmask;
    unsigned int max_pending;

    /* status description */
    unsigned long flags;
    unsigned int used;
    unsigned int pending;

    /* state counter */
    unsigned int changed;
    unsigned long hits;
    unsigned long misses;
    unsigned long starve;
};

GENERIC_STRUCT_BITOPS(lrc, struct lrc_head, flags, false);
GENERIC_STRUCT_FLAG(lrc, struct lrc_head, flags, dirty, __LRC_DIRTY);
GENERIC_STRUCT_FLAG(lrc, struct lrc_head, flags, starving, __LRC_STARVING);

extern struct lrc_node *lrc_kmalloc(void *pdata);
extern void lrc_kfree(struct lrc_node *node, void *pdata);
extern void __init lrc_init(void);

extern struct lrc_node *lrc_find(struct lrc_head *head, unsigned int tag);
extern bool lrc_check_used(struct lrc_head *head, unsigned int tag);
extern struct lrc_node *lrc_obtain(struct lrc_head *head, unsigned int tag, long flags);
extern unsigned int lrc_put(struct lrc_head *head, struct lrc_node *node);
extern state lrc_del(struct lrc_head *head, struct lrc_node *node);
extern void lrc_set(struct lrc_head *head, struct lrc_node *node, unsigned int tag);
extern void lrc_committed(struct lrc_head *head);
extern void lrc_reset(struct lrc_head *head);
extern struct lrc_head *lrc_create(unsigned int number, unsigned int maxpend, lrc_alloc alloc, lrc_free free, void *pdata);
extern void lrc_destroy(struct lrc_head *head, lrc_free free, void *pdata);

static inline struct lrc_node *lrc_get(struct lrc_head *head, unsigned int tag)
{
    return lrc_obtain(head, tag, LRC_CHANGE);
}

static inline struct lrc_node *lrc_try_get(struct lrc_head *head, unsigned int tag)
{
    return lrc_obtain(head, tag, 0);
}

static inline struct lrc_node *lrc_cumulative(struct lrc_head *head, unsigned int tag)
{
    return lrc_obtain(head, tag, LRC_CHANGE | LRC_UNCOMMITTED);
}

#endif  /* _LRU_CACHE_H_ */
