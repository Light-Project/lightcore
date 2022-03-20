/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "slob"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kmalloc.h>
#include <memory.h>
#include <numa.h>
#include <string.h>
#include <export.h>
#include <printk.h>
#include <panic.h>
#include <asm/page.h>
#include <asm/cache.h>

#if (PAGE_SHIFT > 15)
# define slobidx_t   uint32_t
#else
# define slobidx_t   uint16_t
#endif

struct slob_node {
#ifdef CONFIG_DEBUG_SLOB
    uint32_t magic;
#endif
    slobidx_t size;
    bool use;
};

#define SLOB_PAGEUSE NULL
#define SLOB_ALIGN max(CACHE_LINE_SIZE, MSIZE)

static LIST_HEAD(slob_free_small);
static LIST_HEAD(slob_free_medium);
static LIST_HEAD(slob_free_large);
static SPIN_LOCK(lock);

static inline struct slob_node *slob_node_next(struct slob_node *node)
{
    return (struct slob_node *)((size_t)node + node->size);
}

static inline bool slob_node_last(struct slob_node *node)
{
	return !((size_t)slob_node_next(node) & ~PAGE_MASK);
}

static struct page *slob_get_page(int order, gfp_t gfp, int numa)
{
    struct page *page;

#ifdef CONFIG_NUMA
    if (numa != NUMA_NONE)
        page = page_alloc_numa(order, gfp, numa);
    else
#endif
        page = page_alloc(order, gfp);

    if (unlikely(!page))
        return NULL;

    page->type = PAGE_SLOB;
    return page;
}

/**
 * slob_node_find - find a slob node within a given page.
 * @slob_page: page to find node
 * @block: address of node
 * @pnext: used to return next node
 * @pprev: used to return prev node
 * @offset: offset in the allocated block that will be aligned
 */
static struct slob_node *slob_node_find(struct slob_page *slob, const void *block,
                struct slob_node **pnext, struct slob_node **pprev, size_t offset)
{
    struct slob_node *node, *next, *prev = NULL;

    for (node = slob->node;; node = slob_node_next(node)) {
        next = slob_node_next(node);

        if ((void *)node + offset == block)
            break;

        if ((void *)node + offset > block)
            return NULL;

        if (slob_node_last(node))
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
 * @align: allocation alignment
 * @offset: offset in the allocated block that will be aligned
 * @bsize: size of the minimum allocation
 */
static void *slob_page_alloc(struct slob_page *slob_page, size_t size,
                             size_t align, size_t offset, size_t bsize)
{
    struct slob_node *node, *aligned, *free, *prev = NULL;
    slobidx_t used, avail, delta = 0;

    /*
     * SLOB adjustment: (offset = 32, align = 32)
     *
     *  addr:  0      3       32       64
     *  size:         | delta | offset |  size (free)
     *   map:  ++++++ | ##### | ###### | #############
     *  type:   used  |          avail
     */

    for (node = slob_page->node;; node = slob_node_next(node)) {
        avail = node->size;

#ifdef CONFIG_DEBUG_SLOB
        if (DEBUG_DATA_CHECK(node->magic != KMAGIC,
            "page_alloc corruption %p magic (%x) out of bounds\n", node, node->magic))
            return NULL;
#endif

        if (node->use)
            goto skip;

        if (align) {
            aligned = (struct slob_node *)(align_high
                      ((size_t)node + offset, align) - offset);
            delta = (size_t)aligned - (size_t)node;
        }

        /* Is there enough space */
        if (avail >= delta + offset + size)
            break;

skip:
        /* It's the last node. There's no memory available. */
        if (slob_node_last(node))
            return NULL;

        prev = node;
    }

    if (unlikely(delta)) {
        if (delta > (offset + bsize)) {
            /* delta is availability */
            node->size = delta;
            node->use = false;
        } else {
            /* delta is not available */
            if (prev)
                prev->size += delta;
            else
                slob_page->node = aligned;
            slob_page->avail -= delta;
        }
        avail -= delta;
        node = aligned;
        node->size = avail;
#ifdef CONFIG_DEBUG_SLOB
        node->magic = KMAGIC;
#endif
    }

    /* avail = offset + size (free) */
    slob_page->avail -= avail;
    used = offset + size;

    /* avail = free */
    avail -= used;

    /* avoid generating unusable fragments */
    if (avail >= (offset + bsize)) {
        node->size = used;
        free = slob_node_next(node);
        free->size = avail;
        free->use = false;
#ifdef CONFIG_DEBUG_SLOB
        free->magic = KMAGIC;
#endif
        slob_page->avail += avail;
    }

    node->use = true;
    return (void *)node + offset;
}

/**
 * slob_page_free - free memory within a given page.
 * @slob_page: page to free memory
 * @block: address of node
 * @offset: offset in the allocated block that will be aligned
 */
static int slob_page_free(struct slob_page *slob_page, const void *block, size_t offset)
{
    struct slob_node *node, *next, *prev;
    slobidx_t free, delta;

    node = slob_node_find(slob_page, block, &next, &prev, offset);
#ifdef CONFIG_DEBUG_SLOB
    if (DEBUG_DATA_CHECK(!node || !node->use,
        "page_free illegal release %p\n", block))
        return -1;
#endif

    free = node->size;

    /* merge the next free node */
    if (!slob_node_last(node) && !next->use)
        node->size += next->size;

    /* merge the prev free node */
    if (prev && !prev->use)
        prev->size += node->size;

    /* release first node pad */
    if (!prev && !page_ptr_aligned(slob_page->node)) {
        prev = page_address(slob_to_page(slob_page));
        delta = (size_t)slob_page->node - (size_t)prev;
        node = prev;
        node->size = delta + slob_page->node->size;
        slob_page->node = node;
#ifdef CONFIG_DEBUG_SLOB
        node->magic = KMAGIC;
#endif
        free += delta;
    }

    node->use = false;
    slob_page->avail += free;

    return slob_node_last(slob_page->node);
}

static void *slob_alloc_node(struct list_head *slob_list, size_t size, size_t offset,
                             size_t align, size_t bsize, gfp_t flags, int numa)
{
    struct slob_page *slob_page, *next;
    irqflags_t irq_save;
    void *block = NULL;

    spin_lock_irqsave(&lock, &irq_save);

    list_for_each_entry_safe(slob_page, next, slob_list, list) {
        if (slob_page->node == SLOB_PAGEUSE)
            continue;

        /* Is there enough space */
        if (slob_page->avail < offset + size)
            continue;

        block = slob_page_alloc(slob_page, size, align, offset, bsize);
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
        slob_page->head = slob_list;
        slob_page->avail = PAGE_SIZE;
        slob_page->node = page_address(page);
        slob_page->node->size = PAGE_SIZE;
        slob_page->node->use = false;
#ifdef CONFIG_DEBUG_SLOB
        slob_page->node->magic = KMAGIC;
#endif

        /* The new page will be given priority next time */
        list_add(slob_list, &slob_page->list);

        block = slob_page_alloc(slob_page, size, align, offset, bsize);
        spin_unlock_irqrestore(&lock, &irq_save);
    }

    if (flags & GFP_ZERO)
        memset(block, 0, size);

    return block;
}

static void *slob_alloc(size_t size, gfp_t flags, size_t align, int numa)
{
    struct list_head *slob_list;
    unsigned int break_size;

    if (BUG_ON(!size))
        return NULL;

    align = max(align, SLOB_ALIGN);
    size = align_high(size, align);

    if (size < CONFIG_SLOB_MEDIUM_LINE) {
        slob_list = &slob_free_small;
        break_size = SLOB_ALIGN;
    } else if (size < CONFIG_SLOB_HUGE_LINE) {
        slob_list = &slob_free_medium;
        break_size = CONFIG_SLOB_MEDIUM_LINE;
    } else {
        slob_list = &slob_free_large;
        break_size = CONFIG_SLOB_HUGE_LINE;
    }

    return slob_alloc_node(slob_list, size,
        SLOB_ALIGN, align, break_size, flags, numa);
}

static void slob_free(const void *block)
{
    struct page *page = va_to_page(block);
    struct slob_page *slob_page = &page->slob;
    irqflags_t irq_save;
    int retval;

    if (BUG_ON(!block))
        return;

    if (unlikely(page->type != PAGE_SLOB)) {
        pr_crit("Illegal release (%p), not a slob page\n", block);
        return;
    }

    if (page->slob.node == SLOB_PAGEUSE) {
        page_free(page);
        return;
    }

    spin_lock_irqsave(&lock, &irq_save);

    retval = slob_page_free(slob_page, block, SLOB_ALIGN);
    if (retval < 0)
        goto finish;

    if (retval) {
#ifdef CONFIG_DEBUG_SLOB
        if (DEBUG_DATA_CHECK(slob_page->avail != PAGE_SIZE,
            "free corruption (%p)->avail should be equal to (%#04lx), "
            "but was (%#04x)\n", slob_page, PAGE_SIZE, slob_page->avail))
            goto finish;
#endif
        if (!list_check_outsize(&slob_page->list))
            list_del(&slob_page->list);

        spin_unlock_irqrestore(&lock, &irq_save);
        page_free(page);

        return;
    }

    if (list_check_outsize(&slob_page->list))
        list_add(slob_page->head, &slob_page->list);

finish:
    spin_unlock_irqrestore(&lock, &irq_save);
}

/*
 * End of slob allocator proper.
 * Begin kcache and kmalloc frontend.
 */

size_t ksize(const void *block)
{
    struct slob_page *slob_page;
    struct slob_node *node;
    struct page *page;

    if (BUG_ON(!block))
        return 0;

    page = va_to_page(block);
    slob_page = &page->slob;

    if (slob_page->node == SLOB_PAGEUSE)
        return page_size(page);

    node = slob_node_find(&page->slob, block, NULL, NULL, SLOB_ALIGN);
    if (!node)
        return 0;

    return node->size;
}
EXPORT_SYMBOL(ksize);

static struct page *kmalloc_large(size_t size, gfp_t flags, int numa, size_t align)
{
    int order = size_to_order(max(size, align));
    struct page *page;

    page = slob_get_page(order, flags, numa);
    if (unlikely(!page))
        return NULL;

    page->slob.node = SLOB_PAGEUSE;
    return page;
}

void *kmalloc_numa_align(size_t size, gfp_t flags, int numa, size_t align)
{
    struct page *page;
    void *block;

    if (size <= (PAGE_SIZE - SLOB_ALIGN) && align <= (PAGE_SIZE / 2))
        block = slob_alloc(size, flags, align, numa);

    else {
        page = kmalloc_large(size, flags, numa, align);
        block = page_address(page);
    }

    return block;
}
EXPORT_SYMBOL(kmalloc_numa_align);

void *kmalloc_node(size_t size, gfp_t flags, int numa)
{
    return kmalloc_numa_align(size, flags, numa, 0);
}
EXPORT_SYMBOL(kmalloc_node);

void *kmalloc_align(size_t size, gfp_t flags, size_t align)
{
    return kmalloc_numa_align(size, flags, NUMA_NONE, align);
}
EXPORT_SYMBOL(kmalloc_align);

void *kmalloc(size_t size, gfp_t flags)
{
    return kmalloc_numa_align(size, flags, NUMA_NONE, 0);
}
EXPORT_SYMBOL(kmalloc);

void kfree(void *block)
{
    slob_free(block);
}
EXPORT_SYMBOL(kfree);

void *kcache_alloc_numa(struct kcache *cache, gfp_t flags, int numa)
{
    size_t size = cache->objsize;
    struct page *page;
    void *block;

    if (size >= PAGE_SIZE) {
        page = kmalloc_large(size, flags, numa, cache->align);
        list_add_prev(&cache->free, &page->slob.list);
        block = page_address(page);
    } else {
        block = slob_alloc_node(&cache->free, size,
            SLOB_ALIGN, cache->align, size, flags, numa);
    }

    return block;
}
EXPORT_SYMBOL(kcache_alloc_numa);

void *kcache_alloc(struct kcache *cache, gfp_t flags)
{
    return kcache_alloc_numa(cache, flags, NUMA_NONE);
}
EXPORT_SYMBOL(kcache_alloc);

void kcache_free(struct kcache *cache, void *block)
{
    slob_free(block);
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
