/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Virtual memory allocation core
 */

#include <mm/kmem.h>
#include <mm/vmem.h>
#include <system/cpu.h>

#include <export.h>
#include <printk.h>

#include <asm/page.h>

// static RB_ROOT(vm_busy_area_root);
static RB_ROOT(vm_free_area_root);
static LIST_HEAD(vm_area_list);

struct vm_area *vmem_find_area(void *addr)
{
	struct rb_node *rb_node = vm_free_area_root.rb_node;
    struct vm_area *va;
    void *end;

    while(rb_node) {
		va = rb_entry(rb_node, struct vm_area, rb_node);
        end = (void *)(size_t)va->addr + va->size;

        if(addr < va->addr)
            rb_node = rb_node->left;
        else if(end <= addr)
            rb_node = rb_node->right;
        else
            return va;
    }

    return NULL;
}

struct vm_area *vmem_find_free(size_t size, unsigned int align)
{
    struct rb_node *rb_node = vm_free_area_root.rb_node;
    struct vm_area *va;

    /* traversing free nodes */
    while(rb_node) {
        va = rb_entry(rb_node, struct vm_area, rb_node);

        if(va->size < size)
            rb_node = rb_node->right;
        else if(va->size > size)
            rb_node = rb_node->left;
        else
            return va;
    }

    return NULL;
}

/**
 * vmem_alloc - Allocate a virtual memory
 * 
 */
struct vm_area *vmem_alloc(size_t size, gfp_t gfp)
{
    struct vm_area *vm;

    size = align_high(size, PAGE_SIZE);

    vm = kzalloc(sizeof(*vm), gfp);
    if(unlikely(!vm))
        return NULL;

    vmem_find_free(size, 4);


    return vm;

    kfree(vm);
    return NULL;
}
EXPORT_SYMBOL(vmem_alloc);

/**
 * vmem_free - Free up a virtual memory
 * 
 */
void vmem_free(struct vm_area *va)
{
    
}
EXPORT_SYMBOL(vmem_free);

void __init vmem_init(void)
{
    int cpu;
    
    for_each_possible_cpu(cpu) {


    }
    
}
