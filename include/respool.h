/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RESPOOL_H_
#define _RESPOOL_H_

#include <list.h>
#include <spinlock.h>

struct resnode;
struct respool;

typedef void (*respool_release_t)(struct respool *pool, struct resnode *res);
typedef bool (*respool_find_t)(struct respool *pool, struct resnode *res, const void *data);

struct resnode {
    const char *name;
    struct list_head list;
    respool_release_t release;
};

struct respool {
    const char *name;
    spinlock_t lock;
    struct list_head node;
};

#define RESPOOL_STATIC(name) {              \
    .name = __stringify(name),              \
    .lock = SPIN_LOCK_STATIC(name),         \
    .node = LIST_HEAD_STATIC((name).list),  \
}

#define RESPOOL_INIT(name) \
    (struct respool) RESPOOL_STATIC(name)

#define DEFINE_RESPOOL(name) \
    struct respool name = RESPOOL_INIT(name)

static inline void respool_init(struct respool *pool, const char *name)
{
    pool->name = name;
    spin_lock_init(&pool->lock);
    list_head_init(&pool->node);
}

static inline bool respool_check_empty(struct respool *pool)
{
    return list_check_empty(&pool->node);
}

extern struct resnode *respool_find(struct respool *pool, respool_find_t find, const void *data);
extern void respool_insert(struct respool *pool, struct resnode *res);
extern void respool_remove(struct respool *pool, struct resnode *res);
extern void respool_release(struct respool *pool, struct resnode *res);
extern struct resnode *respool_find_remove(struct respool *pool, respool_find_t find, const void *data);
extern struct resnode *respool_find_release(struct respool *pool, respool_find_t find, const void *data);
extern void respool_release_all(struct respool *pool);

#endif /* _RESPOOL_H_ */
