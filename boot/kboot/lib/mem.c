/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  mm/kmem.c
 *  light core BestFit heap manager
 *  (C) 2020 wzl realization
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-01-18      first version 
 * 
 */

#include <lib.h>
#include <list.h>
#include <../../../lib/list/list.c>

#define heap_autograph 0x55aa00ff

void *ld_piggy_start = &_ld_piggy_start;
void *ld_piggy_end = &_ld_piggy_end;
void *ld_bss_start = &_ld_bss_start;
void *ld_bss_end = &_ld_bss_end;
void *ld_heap_start = &_ld_heap_start;
void *ld_heap_end = &_ld_heap_end;

typedef struct{
    void *addr;
    void *end;
    list_t phy_head;
    list_t free_head;
    uint32_t available;
    uint32_t total;
}kmem_heap_t;

typedef struct{
    uint32_t autograph;
    bool used;
    size_t size;
    list_t phy_list;
    list_t free_list;
}kmem_heap_node_t;

/* Save heap address */
static void *mem_heap_addr;

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
    
    return true;

}   

static kmem_heap_node_t *heap_sort(list_t *head, size_t size)
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

    /* get the best free mem block */
    node = heap_sort(&heap->free_head, size);
    
    /* if no suitable memory is found, exit */
    if(node == NULL)
    {
        return NULL;
    }

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
    {
        return;
    }
    
    node->used = false;

    list_add(&heap->free_head, &node->free_list);

    /* Merge previous idle node */
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
}

void heap_init(void *heap_start,size_t heap_size)
{    
    if((heap_start == NULL)||(heap_size = 0))
        panic("Heap initialization error");
    mem_heap_addr = heap_start;
    heap_setup(mem_heap_addr,heap_size);
}

void *malloc(size_t size)
{
    if(size == 0)
        return NULL;
    return heap_alloc(mem_heap_addr, size);
}

void free(void *mem)
{
    if(mem == NULL)
        return;
    heap_free(mem_heap_addr, mem);
}
