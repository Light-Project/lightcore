/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KMALLOC_H_
#define _KMALLOC_H_

#include <bits.h>
#include <list.h>
#include <ncache.h>
#include <mm/gfp.h>

enum kcache_flags {
    KCACHE_PANIC    = BIT(0),
};

struct kcache {
    struct list_head list;
    const char *name;
    struct ncache cache;
    struct list_head free;
    size_t objsize, align;
};

/* allocator function */
extern size_t ksize(const void *block);
extern void __malloc *kmalloc(size_t size, gfp_t flags);
extern void __malloc *kmalloc_numa(size_t size, gfp_t flags, int numa);
extern void __malloc *kmalloc_align(size_t size, gfp_t flags, size_t align);
extern void __malloc *kmalloc_numa_align(size_t size, gfp_t flags, int numa, size_t align);
extern void __malloc *krealloc(const void *block, size_t rsize, gfp_t flags);
extern void kfree(const void *mem);

/* kernel cache allocator */
extern void __malloc *kcache_alloc(struct kcache *, gfp_t flags);
extern void __malloc *kcache_alloc_numa(struct kcache *, gfp_t flags, int numa);
extern void kcache_free(struct kcache *, const void *block);
extern struct kcache *kcache_create(const char *name, size_t size, enum kcache_flags flags);
extern void kcache_release(struct kcache *);
extern void kcache_delete(struct kcache *);

#define KCACHE_CREATE(struct, flags) \
    kcache_create(#struct, sizeof(struct), flags)

/**
 * kzalloc - allocate memory and set to zero.
 * @size: alloc memory size.
 * @flags: the type of memory to allocate.
 */
static inline __malloc void *
kzalloc(size_t size, gfp_t flags)
{
    return kmalloc(size, flags | GFP_ZERO);
}

/**
 * kcache_zalloc - allocate kcache and set to zero.
 * @cache: kcache object to allocate
 * @flags: the type of memory to allocate.
 */
static __always_inline __malloc void *
kcache_zalloc(struct kcache *cache, gfp_t flags)
{
    return kcache_alloc(cache, flags | GFP_ZERO);
}

/**
 * kmalloc_array - allocate memory for an array.
 * @nr: number of elements.
 * @size: single element size.
 * @flags: the type of memory to allocate.
 */
static __always_inline __malloc void *
kmalloc_array(size_t nr, size_t size, gfp_t flags)
{
    return kmalloc(size * nr, flags);
}

/**
 * krealloc_array - reallocate memory for an array.
 * @block: number of elements.
 * @nr: number of elements.
 * @size: single element size.
 * @flags: the type of memory to allocate.
 */
static __always_inline __malloc void *
krealloc_array(void *block, size_t nr, size_t size, gfp_t flags)
{
    return krealloc(block, size * nr, flags);
}

/**
 * kmalloc_array - allocate memory for an array and set to zero.
 * @nr: number of elements.
 * @size: single element size.
 * @flags: the type of memory to allocate.
 */
static __always_inline __malloc void *
kcalloc(size_t nr, size_t size, gfp_t flags)
{
    return kmalloc_array(nr, size, flags | GFP_ZERO);
}

extern void __init kmem_init(void);
extern void __init kcache_init(void);

#endif /* _KMALLOC_H_ */
