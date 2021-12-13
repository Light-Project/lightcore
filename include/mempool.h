/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#include <spinlock.h>
#include <mm/gfp.h>

typedef void *(mempool_alloc_t)(gfp_t flags, void *data);
typedef void (mempool_free_t)(void *block, void *data);

struct mempool {
    spinlock_t lock;
    mempool_alloc_t *alloc;
    mempool_free_t *free;
    void *data;

	void **objects;
    unsigned int demand, curr;
};

extern void *mempool_kmalloc(gfp_t flags, void *size);
extern void mempool_kfree(void *block, void *data);
extern void *mempool_kmalloc(gfp_t flags, void *cache);
extern void mempool_kfree(void *block, void *cache);
extern void *mempool_pages_alloc(gfp_t flags, void *order);
extern void mempool_pages_free(void *block, void *data);

extern struct mempool *mempool_create(unsigned int demand, size_t size, gfp_t flags);
extern struct mempool *mempool_create_node(mempool_alloc_t *, mempool_free_t *, unsigned int demand, gfp_t flags, void *data);
extern state mempool_resize(struct mempool *pool, unsigned int demand, gfp_t flags);
extern void mempool_release(struct mempool *pool);
extern void mempool_delete(struct mempool *pool);

extern void __malloc *mempool_alloc(struct mempool *pool, gfp_t flags);
extern void mempool_free(struct mempool *pool, void *block);

/**
 * kcache_zalloc - allocate kcache and set to zero.
 * @cache: kcache object to allocate
 * @flags: the type of memory to allocate.
 */
static inline __malloc void *
mempool_zalloc(struct mempool *pool, gfp_t flags)
{
    return mempool_alloc(pool, flags | GFP_ZERO);
}

extern void __init mempool_init(void);

#endif  /* _MEMPOOL_H_ */
