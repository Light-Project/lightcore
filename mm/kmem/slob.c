/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Slob memory allocator
 *
 * Copyright(c) 2021
 */

#define pr_fmt(fmt) "slob-core: " fmt

#include <mm.h>
#include <string.h>
#include <printk.h>

#define SLOB_SIZE(size) (sizeof(struct slob_node) + (size))

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

/**
 * slob_page_alloc - Allocates memory within a given page
 * 
 * Because we are in a page allocation, so the size 
 * can not exceed one page
 */
static void *slob_page_alloc(struct slob_page *slob_page, 
                              size_t size, int align, bool *full)
{
    struct slob_node *node;
    slobidx_t avail;

    /* This is the limit of slob allocation, use whole page :) */
    if((PAGE_SIZE == size)||((PAGE_SHIFT == align)&&(size <= PAGE_SIZE))) {
        slob_page->page_use = true;
        return slob_page->node;
    }

    for(node = slob_page->node;; node = slob_node_next(node)) {
        avail = node->size;

        if((avail >= size)&&(!node->use))
            break;
        
        /* It's the last node. There's no memory available */
        if(slob_node_last(node))
            return NULL;
    }

    node->use = 1;

    /* If there is enough rest space, set the next node */
    avail -= size;
    if(avail > sizeof(*node)) {
        node->size = size;
        node = slob_node_next(node);
        node->size = avail;
        node->use = false;
    }

    slob_page->avail -= SLOB_SIZE(size);

    /* If the page is full, remove it from the free list */
    if(!slob_page->avail) {
        list_del(&slob_page->slob_list);
    }

    return node + 1;
}

/**
 * slob_page_free - free memory within a given page
 * 
 * Yeah, I didn't write very well
 */
static bool slob_page_free(struct slob_page *slob_page, void *block)
{
    struct slob_node *node, *next, *prev = NULL;

    /* if use whole page, release this page directly */
    if(slob_page->page_use)
        return true;
    
    for(node = slob_page->node;; node = slob_node_next(node)) {
        next = slob_node_next(node);

        /* find node */
        if(((void *)node < block)&&(block < (void *)next))
            break;

        /* Not this node, save prev node */
        prev = node;

        /* It's the last node. There's no block found */
        if(unlikely(slob_node_last(node))) {
            pr_err("release %llx error\n\r", block);
            return false;
        }
    }
    
    /* set node free at first */
    node->use = false;

    /* Merge the next free node */
    if(!next->use)
        node->size += SLOB_SIZE(next->size);

    /*
     * If it's not the first node, then we merge the 
     * previous free node 
     */
    if(prev && (!prev->use))
        prev->size += SLOB_SIZE(next->size);

    return false;
}

/**
 * slob_alloc - slob alloc core
 * 
 */
void *slob_alloc(size_t size, gfp_t gfp, int align)
{
    list_t *slob_list;
    struct slob_page *slob_page;
    irqflags_t irq_save;
    bool full;
    void *block = NULL;

    /*
     * Slob can only handle memory 
     * no more than one page 
     */
    if(size > PAGE_SIZE)
        return NULL;

    if(size < SLOB_BREAK1)
        slob_list = &slob_free_small;
    else if(size < SLOB_BREAK2)
        slob_list = &slob_free_medium;
    else
        slob_list = &slob_free_large;

    spin_lock_irqsave(&lock, &irq_save);
    
    list_for_each_entry(slob_page, slob_list, slob_list) {
        
        block = slob_page_alloc(slob_page, size, align, &full);
        if(!block)
            continue;

        /*
         * Move to the tail and give priority 
         * to the last used node (LRU) to improve 
         * the fragment distribution
         */
        if(!full)
            if(!list_check_first(slob_list, &slob_page->slob_list))
                list_move_tail(slob_list, &slob_page->slob_list);
    }

    if(block && (gfp & GFP_ZERO))
        memset(block, 0, size);

    /* No enough space in slob list */
    if(!block) {
        struct page *page;
        
        /* Alloc a new page */
        page = page_alloc(0, gfp);
        if(!page)
            return NULL;

        slob_page = &page->slob;

        /* The new assigned page will be given priority next time */
        list_add(slob_list, &slob_page->slob_list);
        slob_page->node = page_to_address(page);
        slob_page->avail = PAGE_SIZE;

        /* Setup the first node */
        slob_page->node->size = PAGE_SIZE - sizeof(struct slob_node);
        slob_page->node->use = false;

        block = slob_page_alloc(slob_page, size, align, &full);
    }

    spin_unlock_irqrestore(&lock, &irq_save);
    
    return block;
}

/**
 * slob_free - slob release core
 * 
 */
void slob_free(void *block)
{
    struct page *page = &va_to_page((size_t)block & PAGE_MASK);
    irqflags_t irq_save;
    
    spin_lock_irqsave(&lock, &irq_save);

    if(slob_page_free(&page->slob, block))
        page_free(page, 0);

    spin_unlock_irqrestore(&lock, &irq_save);
}