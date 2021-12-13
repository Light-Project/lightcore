/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "slob: " fmt

#include <kmalloc.h>
#include <mm.h>
#include <mm/page.h>
#include <numa.h>
#include <string.h>
#include <export.h>
#include <printk.h>
#include <asm/page.h>
#include <asm/cache.h>

#if (PAGE_SHIFT < 15)
#define slobidx_t   uint16_t
#else
#define slobidx_t   uint32_t
#endif

struct slob_node {
    slobidx_t size:PAGE_SHIFT;
    slobidx_t use:1;
};

#define SLOB_PAGEUSE NULL
#define SLOB_SIZE(size) (sizeof(struct slob_node) + (size))

#define SLOB_BREAK1 256
#define SLOB_BREAK2 1024
static LIST_HEAD(slob_free_small);
static LIST_HEAD(slob_free_medium);
static LIST_HEAD(slob_free_large);
static SPIN_LOCK(lock);

static inline struct slob_node *slob_node_next(struct slob_node *node)
{
    return (struct slob_node *)((size_t)(node + 1) + node->size);
}

static inline bool slob_node_last(struct slob_node *node)
{
	return !((size_t)slob_node_next(node) & ~PAGE_MASK);
}

static struct page *slob_get_page(gfp_t gfp, int order, int numa)
{
    struct page *page;

#ifdef CONFIG_NUMA
    if (numa != NUMA_NONE)
        page = page_numa_alloc();
    else
#endif
        page = page_alloc(order, gfp);

    if (unlikely(!page))
        return NULL;

    page->type = PAGE_SLOB;
    return page;
}

static struct slob_node *slob_node_find(struct slob_page *slob, const void *block,
                                        struct slob_node **pnext, struct slob_node **pprev)
{
    struct slob_node *node, *next, *prev = NULL;

    for (node = slob->node;; node = slob_node_next(node)) {
        next = slob_node_next(node);

        if ((void *)node <= block && (void *)next >= block)
            break;

        /* It's the last node. There's no block found */
        if (unlikely(slob_node_last(node)))
            return NULL;

        prev = node;
    }

    if (pnext)
        *pnext = next;

    if (pprev)
        *pprev = prev;

    return node;
}

/**
 * slob_page_alloc - allocates memory within a given page.
 * @slob_page: page to allocate memory
 * @size: size of the allocation
 * @align: minimum limit of allocate
 * @offset: offset in the allocated block that will be aligned
 * @bsize: size of the minimum allocation
 */
static void *slob_page_alloc(struct slob_page *slob_page, size_t size,
                             size_t align, size_t offset, size_t bsize)
{
    struct slob_node *node, *aligned, *free, *prev = NULL;
    slobidx_t used, avail, delta = 0;

    for (node = slob_page->node;; node = slob_node_next(node)) {
        avail = node->size;

        if (align) {
            aligned = (struct slob_node *)(align_high
                    ((size_t)node + offset, align) - offset);
            delta = aligned - node;
        }

        /* Is there enough space */
        if (!node->use && avail >= delta + size)
            break;

        /* It's the last node. There's no memory available. */
        if (slob_node_last(node))
            return NULL;

        prev = node;
    }

    if (unlikely(delta)) {
        if (delta > (offset + bsize))
            node->size = delta - sizeof(*node);
        else if (bsize && prev)
            prev->size += delta;
        aligned->size = avail - delta;
        node = aligned;
    }

    slob_page->avail -= avail;
    used = offset + size;
    avail -= used;

    /* avoid generating unusable fragments */
    if (avail > (offset + bsize)) {
        node->size = used - sizeof(*node);
        free = slob_node_next(node);
        free->use = false;
        free->size = avail;
        slob_page->avail += avail;
    }

    node->use = true;
    return (void *)node + offset;
}

/**
 * slob_page_free - free memory within a given page.
 * @slob_page: page to free memory
 * @block: address pointer
 */
static bool slob_page_free(struct slob_page *slob_page, const void *block)
{
    struct slob_node *node, *next, *prev;
    slobidx_t free;

    node = slob_node_find(slob_page, block, &next, &prev);
    node->use = false;
    free = node->size;

    /* merge the next free node */
    if (!slob_node_last(node) && !next->use) {
        node->size += SLOB_SIZE(next->size);
        free += sizeof(*next);
    }

    /* merge the prev free node */
    if (prev && !prev->use) {
        prev->size += SLOB_SIZE(node->size);
        free += sizeof(*node);
    }

    slob_page->avail += free;
    return slob_node_last(slob_page->node);
}

static void *slob_alloc_node(struct list_head *slob_list, size_t size,
                             size_t align, size_t bsize, gfp_t flags, int numa)
{
    size_t minalign = max(CACHE_LINE_SIZE, MSIZE);
    struct slob_page *slob_page, *next;
    irqflags_t irq_save;
    void *block = NULL;

    align = max(align, minalign);
    size  = align_high(size, align);
    bsize = align_high(bsize, align);

    spin_lock_irqsave(&lock, &irq_save);

    list_for_each_entry_safe(slob_page, next, slob_list, list) {
        if (slob_page->node == SLOB_PAGEUSE)
            continue;

        /* Is there enough space */
        if (slob_page->avail < minalign + size)
            continue;

        block = slob_page_alloc(slob_page, size, align, minalign, bsize);
        if (!block)
            continue;

        /*
         * Move to the front and give priority
         * to the last used node (LRU) to improve
         * the fragment distribution
         */
        if (slob_page->avail) {
            if (!list_check_first(slob_list, &slob_page->list))
                list_move_front(slob_list, &slob_page->list);
        } else
            list_del(&slob_page->list);

        break;
    }

    spin_unlock_irqrestore(&lock, &irq_save);

    /* No enough space in slob list */
    if (!block) {
        struct page *page;

        /* Alloc a new page */
        page = slob_get_page(0, flags & ~GFP_ZERO, numa);
        if (unlikely(!page))
            return NULL;
        slob_page = &page->slob;

        spin_lock_irqsave(&lock, &irq_save);

        /* Setup new page */
        slob_page->node = page_address(page);
        slob_page->avail = PAGE_SIZE - sizeof(struct slob_node);
        slob_page->node->size = PAGE_SIZE - sizeof(struct slob_node);
        slob_page->node->use = false;

        /* The new page will be given priority next time */
        list_add(slob_list, &slob_page->list);

        block = slob_page_alloc(slob_page, size, align, minalign, bsize);
        spin_unlock_irqrestore(&lock, &irq_save);
    }

    if (flags & GFP_ZERO)
        memset(block, 0, size);

    return block;
}

static void *slob_alloc(size_t size, gfp_t flags, int align, int numa)
{
    struct list_head *slob_list;
    unsigned int break_size;

    if (size < SLOB_BREAK1) {
        slob_list = &slob_free_small;
        break_size = MSIZE;
    } else if (size < SLOB_BREAK2) {
        slob_list = &slob_free_medium;
        break_size = SLOB_BREAK1;
    } else {
        slob_list = &slob_free_large;
        break_size = SLOB_BREAK2;
    }

    return slob_alloc_node(slob_list, size, align, break_size, flags, numa);
}

static struct page *kmalloc_large(size_t size, gfp_t gfp, int numa)
{
    int order = size_to_order(size);
    struct page *page;

    page = slob_get_page(order, gfp, numa);
    if (unlikely(!page))
        return NULL;

    page->slob.node = SLOB_PAGEUSE;
    return page;
}

size_t ksize(const void *block)
{
    struct slob_page *slob_page;
    struct slob_node *node;
    struct page *page;

    page = va_to_page(block);
    slob_page = &page->slob;

    if (slob_page->node == SLOB_PAGEUSE)
        return page_size(page);

    node = slob_node_find(&page->slob, block, NULL, NULL);
    if (!node)
        return 0;

    return node->size;
}
EXPORT_SYMBOL(ksize);

void *kmalloc(size_t size, gfp_t gfp)
{
    struct page *page;
    void *block;

    /*
     * Slob can only handle memory
     * no more than one page
     */
    if (size >= PAGE_SIZE) {
        page = kmalloc_large(size, gfp, NUMA_NONE);
        block = page_address(page);
    } else
        block = slob_alloc(size, gfp, 0, NUMA_NONE);

    return block;
}
EXPORT_SYMBOL(kmalloc);

void kfree(const void *block)
{
    struct page *page = va_to_page(block);
    struct slob_page *slob_page = &page->slob;
    irqflags_t irq_save;
    bool empty;

    if (unlikely(page->type != PAGE_SLOB)) {
        pr_crit("illegal release page %p\n", block);
        return;
    }

    spin_lock_irqsave(&lock, &irq_save);

    if (page->slob.node == SLOB_PAGEUSE)
        page_free(page);
    else {
        empty = slob_page_free(slob_page, block);
        if (empty) {
            list_del(&slob_page->list);
            page_free(page);
        }
    }

    spin_unlock_irqrestore(&lock, &irq_save);
}
EXPORT_SYMBOL(kfree);

void *kcache_alloc(struct kcache *cache, gfp_t flags)
{
    size_t size = cache->objsize;
    struct page *page;
    void *block;

    if (size >= PAGE_SIZE) {
        page = kmalloc_large(size, flags, NUMA_NONE);
        list_add_prev(&cache->free, &page->slob.list);
        block = page_address(page);
    } else {
        block = slob_alloc_node(&cache->free, size,
                cache->align, size, flags, NUMA_NONE);
    }

    return block;
}
EXPORT_SYMBOL(kcache_alloc);

void kcache_free(struct kcache *cache, void *block)
{
    kfree(block);
}
EXPORT_SYMBOL(kcache_free);

void kcache_release(struct kcache *cache)
{
    struct slob_page *slob, *next;
    struct page *page;

    list_for_each_entry_safe(slob, next, &cache->free, list) {
        page = slob_to_page(slob);
        page_free(page);
        list_del(&slob->list);
    }
}
EXPORT_SYMBOL(kcache_release);
