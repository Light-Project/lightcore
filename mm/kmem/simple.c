/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Simple memory allocator
 * (C) 2020 wzl realization
 */

#include <mm.h>
#include <stddef.h>
#include <string.h>
#include <system/spinlock.h>
#include <export.h>
#include <printk.h>

/**
 * setup the heap memory at boot
 * 
 * @defgroup Memory Manager Block 
 *
 *
 * @param Start_addr: Heap memory start address
 * @param Heap_size Heap memory size
 * 
 * @return ... 
 */
static inline uint8_t heap_setup(void *heap_addr,size_t heap_size)
{
    kmem_heap_t *heap = (kmem_heap_t *) heap_addr;
    kmem_heap_node_t *node;
    
    spin_lock(&heap->lock);
    
    heap->addr      = heap_addr;
    heap->end       = heap_addr + heap_size;
    heap->total     = heap_size;
    
    heap->available = heap->total - 
                    (sizeof(kmem_heap_t) + 
                    sizeof(kmem_heap_node_t));
    
    list_head_init(&heap->phy_head);
    list_head_init(&heap->free_head);
    
    /* setup first free node */
    node            = (kmem_heap_node_t *)
                      heap + sizeof(kmem_heap_node_t); 

    node->autograph = heap_autograph;
    node->used      = false;
    node->size      = heap->available - sizeof(kmem_heap_node_t);
    
    list_add(&heap->phy_head, &node->phy_list);
    list_add(&heap->free_head, &node->free_list);
    
    spin_unlock(&heap->lock);
    
    return true;

}   

#ifdef CONFIG_MM_SIMPLE_FIRST
#define heap_find_free heap_first

/**
 * heap_first - Find the first free node form the free list
 * @head: Heap memory start address
 * @size: Heap memory size
 * 
 * @return: Free nodes found
 */
static kmem_heap_node_t *heap_first(list_t *head, size_t size)
{
    kmem_heap_node_t *node = NULL;
    
    list_for_each_entry(node,head,free_list)
    {
        if(node->size >= size)
            break;
    }
    return node;
}

#elif CONFIG_MM_SIMPLE_BEST
#define heap_find_free heap_best_sort

/**
 * heap_best_sort - Find the largest free node form the free list
 * @head: Heap memory start address
 * @size: Heap memory size
 * 
 * @return: Free nodes found
 */
static kmem_heap_node_t *heap_best_sort(list_t *head, size_t size)
{
    kmem_heap_node_t *node = NULL;
    kmem_heap_node_t *sort = NULL;
    
    list_for_each_entry(sort,head,free_list)
    {
        if(sort->size >= size)
            if((node == NULL)||(node->size > sort->size))
                node = sort;
    }
    return node;
}

#elif CONFIG_MM_SIMPLE_WORST
#define heap_find_free heap_worst_sort
/**
 * heap_worst_sort - Find the smallest free node form the free list
 * @head: Heap memory start address
 * @size: Heap memory size
 * 
 * @return: Free nodes found
 */
static kmem_heap_node_t *heap_worst_sort(list_t *head, size_t size)
{
    kmem_heap_node_t *node = NULL;
    kmem_heap_node_t *sort = NULL;
    
    list_for_each_entry(sort,head,free_list)
    {
        if(sort->size >= size)
            if((node == NULL)||(node->size < sort->size))
                node = sort;
    }
    return node;
}

#endif /* CONFIG_MM_SIMPLE_WORST */


/**
 * setup the heap memory at boot
 * 
 * @defgroup Memory Manager Block 
 *
 *
 * @param head: Heap memory start address
 * @param size Heap memory size
 * 
 * @return ... 
 */
static kmem_heap_node_t *heap_get_node(list_t *head, void *addr)
{
    kmem_heap_node_t *node = NULL;
    list_for_each_entry(node,head,phy_list)
    {
        if(((void *)node < addr) &&
        (addr < (void *)node->phy_list.next))
            return node;
    }
    return NULL;
}

/**
 * Allocating heap memory
 * 
 * @defgroup Memory Manager Block 
 *
 * @param size: Apply heap memory size
 * 
 * @return pointer to the apply heap memory or NULL 
 *         -1: error occurred 
 */
static inline void *heap_alloc(void *heap_addr, size_t size)
{
    kmem_heap_t *heap = heap_addr;
    kmem_heap_node_t *node = NULL;
    
    if(size > heap->available)
        return NULL;

    /* get the free mem block */
    node = heap_find_free(&heap->free_head, size);
    
    /* if no suitable memory is found, exit */
    if(node == NULL)
        return NULL;

    /* lock heap */
    spin_lock(&heap->lock);

    if(node->size == size)
        goto change;

    else if(node->size > size)
    {
        kmem_heap_node_t *free = (kmem_heap_node_t *)NULL;
        
        /* allocation free block */
        free = (kmem_heap_node_t *)
               ((uint8_t *)node + sizeof(kmem_heap_node_t) + size);

        heap->available -= sizeof(kmem_heap_node_t); 

        /* setup the new free block */
        free->autograph = heap_autograph;
        free->size = node->size - size - sizeof(kmem_heap_node_t);
        free->used = false;

        /* add free block to the free list */
        list_add(&heap->free_head, &free->free_list);

        /* add free block to the phy list */
        list_add(&node->phy_list, &free->phy_list);
        goto change;
    }

change:
    list_del(&node->free_list);
    
    /* Adjust heap available size */
    heap->available -= size;

    /* set used node */ 
    node->size = size;
    node->used = true;

    /* unlock heap */
    spin_unlock(&heap->lock);
    return (uint8_t *)node + sizeof(kmem_heap_node_t);
}

/**
 * Free heap memory
 * 
 * @defgroup Memory Manager Block 
 *
 *
 * @param size: Apply heap memory size
 * 
 * @return: addr: success
 *          NULL: error
 */
static inline void heap_free(void *heap_addr,void *mem)
{
    kmem_heap_t *heap = heap_addr;
    kmem_heap_node_t *node = NULL;

    /* Find the freed memory node header */
    node = heap_get_node(&heap->phy_head, mem);

    /* If can't find node, return */
    if(node == NULL)
        return;

    spin_lock(&heap->lock);
    
    node->used = false;

    list_add(&heap->free_head, &node->free_list);

    /* Merge prev idle node */
    if(list_prev_entry(node, phy_list)->used == false)
    {
        kmem_heap_node_t *prev = 
            list_prev_entry(node, phy_list);

        /* Scale forward */
        prev->size += sizeof(kmem_heap_node_t) + node->size;

        /* Remove node from free list */
        list_del(&node->free_list);
        /* Remove node from phy list */
        list_del(&node->phy_list);

        /* Substitute node */
        node = prev;
    }

    /* Merge next idle node */
    if(list_next_entry(node, phy_list)->used == false)
    {
        kmem_heap_node_t *next = 
            list_next_entry(node, phy_list);

        /* Scale back */
        node->size += sizeof(kmem_heap_node_t) + next->size; 

        /* Remove node from free list */
        list_del(&next->free_list); 
        /* Remove node from free list */
        list_del(&next->phy_list);   
    }

    spin_unlock(&heap->lock);
}

static inline bool heap_check(void *heap_addr)
{
    kmem_heap_t *heap = heap_addr;
    kmem_heap_node_t *node = NULL;
    
    list_for_each_entry(node, &heap->phy_head, phy_list)
        if(node->autograph != heap_autograph)
                return false;
    return true;
}

/**
 * External function
 */
static void* mem_heap_addr;

void kmem_init(void)
{    
    size_t heap_size  = align_low(CONFIG_HEAP_SIZE, sizeof(size_t));
    
    mem_heap_addr = (void *)&_ld_bss_end;
    if(mem_heap_addr == NULL && heap_size == 0)
        return;
    heap_setup(mem_heap_addr,heap_size);
}
EXPORT_SYMBOL(kmem_init);

bool kmem_check(size_t size)
{
    return heap_check(mem_heap_addr);
}
EXPORT_SYMBOL(kmem_check);

void *kmalloc(size_t size, gfp_t gfp)
{
    if(size == 0)
        return NULL;
    return heap_alloc(mem_heap_addr, size);
}
EXPORT_SYMBOL(kmalloc);

void* kcalloc(size_t num,size_t size, gfp_t gfp)
{
    void *mem = NULL;
    if((mem = kmalloc(num * size, gfp)))
        memset(mem, 0, num * size);
    return mem;
}
EXPORT_SYMBOL(kcalloc);

void* krealloc(void *mem, size_t size, gfp_t gfp)
{
    if(mem == NULL)
        return heap_alloc(mem_heap_addr, size);

    if(size == 0)
    {
        heap_free(mem_heap_addr, mem);
        return NULL;
    }
    return NULL;
}
EXPORT_SYMBOL(krealloc);

void kfree(void *mem)
{
    if(mem == NULL)
        return;
    heap_free(mem_heap_addr, mem);
}
EXPORT_SYMBOL(kfree);

void kmalloc_info(void)
{
    kmem_heap_t *heap = (kmem_heap_t *)mem_heap_addr;
    kmem_heap_node_t *node = NULL;
    uint32_t count = 0;
    printk("heap info:\n\r");
    printk("          heap_addr: 0x%x\n\r",heap->addr);
    printk("          heap_end:  0x%x\n\r",heap->end);
    printk("          total:     0x%x\n\r",heap->total);
    printk("          available: 0x%x\n\r",heap->available);
    list_for_each_entry(node, &heap->phy_head, phy_list)
    {
        printk("phy node %d info:\n\r",count++);
        printk("          node_addr: 0x%x\n\r",node);
        printk("          node_size: 0x%x\n\r",node->size);
        printk("          node_used: 0x%d\n\r",node->used);
    }
    count = 0;
    list_for_each_entry(node, &heap->free_head, free_list)
    {
        printk("free node %d info:\n\r",count++);
        printk("          node_addr: 0x%x\n\r",node);
        printk("          node_size: 0x%x\n\r",node->size);
        printk("          node_used: 0x%x\n\r",node->used);
    }
}
EXPORT_SYMBOL(kmalloc_info);
