/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "slob: " fmt

#include <string.h>
#include <mm.h>
#include <mm/page.h>
#include <kmalloc.h>
#include <numa.h>
#include <export.h>
#include <printk.h>
#include <asm/page.h>

// #if (PAGE_SHIFT < 15)
// #define slobidx_t   uint16_t
// #else
// #define slobidx_t   uint32_t
// #endif

#define slobidx_t   uint32_t

struct slob_node {
    slobidx_t size:31;
    slobidx_t use:1;
    char data[0];
};

#define SLOB_SIZE(size) (sizeof(struct slob_node) + (size))
#define SLOB_PAGEUSE    NULL

#define SLOB_BREAK1 256
#define SLOB_BREAK2 1024
static LIST_HEAD(slob_free_small);
static LIST_HEAD(slob_free_medium);
static LIST_HEAD(slob_free_large);
static struct spinlock lock;

static __always_inline
struct slob_node *slob_node_next(struct slob_node *node)
{
    return (struct slob_node *)((size_t)(node + 1) + node->size);
}

static __always_inline
bool slob_node_last(struct slob_node *node)
{
	return !((size_t)slob_node_next(node) & ~PAGE_MASK);
}

static struct slob_node *slob_node_find(struct slob_page *slob,
                    const void *block, struct slob_node **pprev)
{
    struct slob_node *node, *prev, *next;

    for (node = slob->node;; node = slob_node_next(node)) {
        next = slob_node_next(node);

        /* find node */
        if (((void *)node < block) && (block < (void *)next))
            break;

        /* Not this node, save prev node */
        prev = node;

        /* It's the last node. There's no block found */
        if (unlikely(slob_node_last(node)))
            return NULL;
    }

    if (pprev)
        *pprev = prev;
    return node;
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

/**
 * slob_page_alloc - Allocates memory within a given page
 * @slob_page: page to allocate memory
 * @size: allocate size
 * @bsize: minimum limit of allocate
 * @align:
 */
static void *slob_page_alloc(struct slob_page *slob_page,
                        size_t size, int bsize, int align)
{
    struct slob_node *node, *free;
    slobidx_t avail;

    for (node = slob_page->node;; node = slob_node_next(node)) {
        avail = node->size;

        if ((avail >= size) && (!node->use))
            break;

        /* It's the last node. There's no memory available. */
        if (slob_node_last(node))
            return NULL;
    }

    node->use = true;
    avail -= size;

    /* avoid generating unusable fragments */
    if (avail > SLOB_SIZE(bsize)) {
        node->size = size;
        free = slob_node_next(node);
        free->size = avail - sizeof(*free);
        free->use = false;
    }

    slob_page->avail -= SLOB_SIZE(node->size);
    return node->data;
}

/**
 * slob_page_free - free memory within a given page
 * @slob_page:
 * @block:
 */
static bool slob_page_free(struct slob_page *slob_page, const void *block)
{
    struct slob_node *node, *next, *prev = NULL;

    for (node = slob_page->node;; node = slob_node_next(node)) {
        next = slob_node_next(node);

        /* find node */
        if (node->data == block)
            break;

        /* Not this node, save prev node */
        prev = node;

        /* It's the bad node. There's no block found. */
        if (unlikely(block < (void *)slob_page->node || slob_node_last(node))) {
            pr_crit("release %p error\n", block);
            return false;
        }
    }

    node->use = false;
    slob_page->avail += node->size;

    /* Merge the next free node */
    if (!slob_node_last(node) && !next->use) {
        node->size += SLOB_SIZE(next->size);
        slob_page->avail += sizeof(*next);
    }

    /* Merge the prev free node */
    if (prev && !prev->use) {
        prev->size += SLOB_SIZE(node->size);
        slob_page->avail += sizeof(*node);
    }

    if (slob_node_last(slob_page->node))
        return true;

    return false;
}

static void *slob_alloc_node(struct list_head *slob_list, unsigned int bsize,
                             size_t size, gfp_t flags, int align, int numa)
{
    struct slob_page *slob_page;
    irqflags_t irq_save;
    void *block = NULL;

    size = align_high(size, max(align, MSIZE));

    spin_lock_irqsave(&lock, &irq_save);

    list_for_each_entry(slob_page, slob_list, slob_list) {
        if (slob_page->node == SLOB_PAGEUSE)
            continue;

        /* Is there enough space */
        if (slob_page->avail < SLOB_SIZE(size))
            continue;

        block = slob_page_alloc(slob_page, size, bsize, align);
        if (!block)
            continue;

        /*
         * Move to the tail and give priority
         * to the last used node (LRU) to improve
         * the fragment distribution
         */
        if (slob_page->avail) {
            if (!list_check_first(slob_list, &slob_page->slob_list))
                list_move_tail(slob_list, &slob_page->slob_list);
        } else
            list_del(&slob_page->slob_list);
    }

    /* No enough space in slob list */
    if (!block) {
        struct page *page;

        /* Alloc a new page */
        page = slob_get_page(0, flags & ~GFP_ZERO, numa);
        if (!page)
            return NULL;

        slob_page = &page->slob;

        /* The new assigned page will be given priority next time */
        list_add(slob_list, &slob_page->slob_list);

        slob_page->node = page_address(page);
        slob_page->avail = PAGE_SIZE - sizeof(struct slob_node);
        slob_page->node->size = PAGE_SIZE - sizeof(struct slob_node);
        slob_page->node->use = false;

        block = slob_page_alloc(slob_page, size, bsize, align);
    }

    if (flags & GFP_ZERO)
        memset(block, 0, size);

    spin_unlock_irqrestore(&lock, &irq_save);
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

    return slob_alloc_node(slob_list, break_size, size, flags, align, numa);
}

size_t ksize(const void *block)
{
    struct page *page = va_to_page(block);
    struct slob_node *node;

    node = slob_node_find(&page->slob, block, NULL);
    if (!node)
        return 0;

    return node->size;
}

static struct page *kmalloc_large(size_t size, gfp_t gfp, int numa)
{
    int order = to_order(size);
    struct page *page;

    page = slob_get_page(order, gfp, numa);
    if (unlikely(!page))
        return NULL;

    page->slob.node = SLOB_PAGEUSE;
    return page;
}

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

void kfree(const void *block)
{
    struct page *page = va_to_page(block);
    struct slob_page *slob_page = &page->slob;
    irqflags_t irq_save;
    bool empty;

    if (page->type != PAGE_SLOB)
        return;

    spin_lock_irqsave(&lock, &irq_save);

    if (page->slob.node == SLOB_PAGEUSE)
        page_free(page);
    else {
        empty = slob_page_free(slob_page, block);
        if (empty) {
            list_del(&slob_page->slob_list);
            page_free(page);
        }
    }

    spin_unlock_irqrestore(&lock, &irq_save);
}

void *kcache_alloc(struct kcache *cache, gfp_t flags)
{
    size_t size = cache->objsize;
    struct page *page;
    void *block;

    if (size >= PAGE_SIZE) {
        page = kmalloc_large(size, flags, NUMA_NONE);
        list_add_prev(&cache->free, &page->slob.slob_list);
        block = page_address(page);
    } else
        block = slob_alloc_node(&cache->free, size,
            size, flags, cache->align, NUMA_NONE);

    return block;
}

void kcache_free(struct kcache *cache, void *block)
{
    kfree(block);
}

void kcache_release(struct kcache *cache)
{
    struct slob_page *slob, *next;
    struct page *page;

    list_for_each_entry_safe(slob, next, &cache->free, slob_list) {
        page = slob_to_page(slob);
        page_free(page);
        list_del(&slob->slob_list);
    }
}

EXPORT_SYMBOL(ksize);
EXPORT_SYMBOL(kmalloc);
EXPORT_SYMBOL(kfree);
EXPORT_SYMBOL(kcache_alloc);
EXPORT_SYMBOL(kcache_free);
EXPORT_SYMBOL(kcache_release);
