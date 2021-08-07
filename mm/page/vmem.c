/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "vm: " fmt

#include <state.h>
#include <mm/kmem.h>
#include <mm/vmem.h>
#include <mm/memmodel.h>
#include <kernel/cpu.h>
#include <export.h>
#include <printk.h>

#include <asm/page.h>

static SPIN_LOCK(vm_area_lock);
static RB_ROOT(vm_area_root);
static LIST_HEAD(vm_free_area_list);

/**
 * area_parent - find the parent of the current node.
 * @va: node to be looked for
 * @root: tree root
 * @parentp: parent pointer
 */
static inline struct rb_node **
area_parent(struct vm_area *va, struct rb_root *root,
            struct rb_node **parentp)
{
    struct vm_area *parent;
    struct rb_node **link;

    link = &root->rb_node;

    if (unlikely(!*link)) {
        *parentp = NULL;
        return link;
    }

    do {
        parent = vm_rb_entry(*link);
        
        if (va->addr < (parent->addr + parent->size) && 
           (va->addr + va->size) <= parent->addr)
            link = &(*link)->left;
        else if ((va->addr + va->size) > parent->addr && 
                  va->addr >= (parent->addr + parent->size))
            link = &(*link)->right;
        else {
            pr_err("area overlaps\n");
            return NULL;
        }
    } while(*link);

    *parentp = &parent->rb_node;
    return link;
}

static inline state 
area_insert(struct vm_area *va, struct rb_root *root, 
            struct list_head *head, bool free)
{
    struct rb_node *parent, **link;

    link = area_parent(va, root, &parent);
    if(unlikely(!link))
        return -EFAULT;

    if (likely(parent)) {
        head = &vm_rb_entry(parent)->list;
        if(parent->right == *link)
            head = head->prev;
    }

    spin_lock(&vm_area_lock);

    rb_link(parent, link, &va->rb_node);
    rb_fixup(root, &va->rb_node);

    if (free)
        list_add(head, &va->list);

    spin_unlock(&vm_area_lock);

    return -ENOERR;
}

static __always_inline void 
area_del(struct vm_area *va, struct rb_root *root)
{
    rb_del(root, &va->rb_node);
}

/**
 * area_split - 
 */
static __always_inline state
area_split(struct vm_area *va, size_t align_start, size_t size)
{

    /* Node not within va */
    if(va->addr > align_start || va->size < size)
        return -EINVAL;

    if (va->addr == align_start) {
        if(va->size == size) {		
            /*
             * No need to split VA, it fully fits.
             *
             * |               |
             * V      NVA      V
             * |---------------|
             */
            area_del(va, &vm_area_root);
            kfree(va);
        } else {
            /*
             * Split left edge of fit VA.
             *
             * |       |
             * V  NVA  V   R
             * |-------|-------|
             */
            va->addr = align_start + size;
            va->size -= size;
        }
    } else if (va->addr + va->size == align_start + size) {
        /*
         * Split right edge of fit VA.
         *
         *         |       |
         *     L   V  NVA  V
         * |-------|-------|
         */
        va->size -= size;
    } else {
        /*
         * Split no edge of fit VA.
         *
         *     |       |
         *   L V  NVA  V R
         * |---|-------|---|
         */

    }

    return -ENOERR;
}

/**
 * vmem_find_free - find the first free node in list
 * @size: allocation size
 * @align: 
 */
static inline size_t 
free_area_find(size_t size, unsigned int align, struct vm_area **vap)
{
    struct vm_area *va;

    /* traversing free nodes */
    list_for_each_entry(va, &vm_free_area_list, list) {
        size_t align_start, start = va->addr;

        align_start = align_high(start, align);

        /* Prevent overflow caused by too large address. */
        if (align_start + size < align_start || align_start < start)
            continue;

        if (align_start + size <= start + va->size) {
            *vap = va;
            return align_start;
        }
    }

    return 0;
}

static struct vm_area *vmem_area_alloc(size_t size, size_t align, gfp_t gfp)
{
    struct vm_area *va, *free;
    size_t addr;

    va = kmalloc(sizeof(*va), gfp);
    if (unlikely(!va))
        return NULL;

    addr = free_area_find(size, align, &free);
    if (unlikely(!addr))
        goto fail;

    area_split(free, addr, size);

    va->addr = addr;
    va->size = size;

    area_insert(va, &vm_area_root, &vm_free_area_list, false);

    return va;

fail:
    kfree(va);
    return NULL;
}

/**
 * area_find - finding nodes by address
 * @addr: address to find
 */
struct vm_area *vmem_area_find(void *addr)
{
    struct rb_node *rb_node;
    struct vm_area *va;

    rb_node = vm_area_root.rb_node;
    while (rb_node) {
        va = rb_entry(rb_node, struct vm_area, rb_node);
        
        if ((size_t)addr < va->addr)
            rb_node = rb_node->right;
        else if ((size_t)addr > va->addr + va->size)
            rb_node = rb_node->left;
        else
            return va;
    }

    return NULL;
}

struct vm_area *vmem_alloc_node(size_t size, size_t align, gfp_t gfp)
{
    size = align_high(size, PAGE_SIZE);
    if (unlikely(!size))
        return NULL;
    return vmem_area_alloc(size, align, gfp);
}

struct vm_area *vmem_alloc(size_t size)
{
    return vmem_alloc_node(size, PAGE_SIZE, GFP_KERNEL);
}

state vmem_free(struct vm_area *va)
{
    return -ENOERR;
}

void __weak __init vmem_init(void)
{
    struct vm_area *vm;

    vm = kzalloc(sizeof(*vm), GFP_KERNEL);    
    if (unlikely(!vm)) {
        panic("vmem initialization failed");
    }

    vm->addr = CONFIG_HIGHMAP_OFFSET;
    vm->size = 0x10000000 - 1;

    area_insert(vm, &vm_area_root, &vm_free_area_list, true);
}
