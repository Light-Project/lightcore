/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _NCACHE_H_
#define _NCACHE_H_

#include <bits.h>
#include <spinlock.h>
#include <mm/gfp.h>
#include <atomic.h>

typedef void *(*ncache_alloc_t)(void *pdata);
typedef void (*ncache_free_t)(void *object, void *pdata);

enum ncache_flags {
    NCACHE_PANIC    = BIT(0),
    NCACHE_NOFULL   = BIT(1),
};

struct ncache {
    spinlock_t lock;
    enum ncache_flags flags;
    ncache_alloc_t alloc;
    ncache_free_t free;
    void *pdata;

    atomic_t curr;
    unsigned long num;
    void *xchg_cache;
    void *objects[0];
};

extern void *ncache_get(struct ncache *cache);
extern void ncache_put(struct ncache *cache, void *object);
extern state ncache_charge(struct ncache *cache);
extern void ncache_flush(struct ncache *cache);
extern struct ncache *ncache_create(ncache_alloc_t, ncache_free_t, unsigned int num, enum ncache_flags, void *pdata);
extern void ncache_destroy(struct ncache *cache);

#endif /* _NCACHE_H_ */
