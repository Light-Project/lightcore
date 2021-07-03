/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_KMEM_H_ 
#define _MM_KMEM_H_

// #include <mm/slab.h>
#include <mm/slob.h>

#include <list.h>
#include <system/spinlock.h>

#define heap_autograph 0x55aa00ff

#ifndef __ASSEMBLY__

typedef struct{
    spinlock_t lock;
    void *addr;
    void *end;
    list_t phy_head;
    list_t free_head;
    uint32_t available;
    uint32_t total;
}kmem_heap_t;

typedef struct{
    uint32_t autograph;
    spinlock_t lock;
    bool used;
    size_t size;
    list_t phy_list;
    list_t free_list;
}kmem_heap_node_t;

#define kzalloc(size, gfp)  kmalloc(size, gfp | GFP_ZERO)

void kmem_init(void);
void *kmalloc(size_t size, gfp_t);
void *kcalloc (size_t nmemb, size_t size, gfp_t);
void *krealloc(void* mem, size_t size, gfp_t);
void kfree(void *mem);
void kmalloc_info(void);

#endif	/* __ASSEMBLY__ */
#endif /* _MM_KMEM_H_ */
