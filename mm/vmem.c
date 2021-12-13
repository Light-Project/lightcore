/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "vmem: " fmt

#include <mm/vmem.h>
#include <kmalloc.h>
#include <kernel.h>
#include <spinlock.h>
#include <export.h>
#include <printk.h>
#include <asm/page.h>

static SPIN_LOCK(vm_area_lock);
static RB_ROOT(vm_area_root);
static LIST_HEAD(vm_free_area_list);
static struct kcache *vmem_cache;

static long vmem_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    struct vm_area *vma_a = rb_to_vma(rba);
    struct vm_area *vma_b = rb_to_vma(rbb);
    return vma_a->addr - vma_b->addr;
}

static long vmem_rb_find(const struct rb_node *rb, const void *key)
{
    struct vm_area *vma = rb_to_vma(rb);

    if ((size_t)key < vma->addr)
        return -1;
    if ((size_t)key > vma->addr + vma->size)
        return 1;

    return 0;
}

struct vm_area *vmem_area_find(size_t addr)
{
    struct rb_node *rb;
    rb = rb_find(&vm_area_root, (void *)addr, vmem_rb_find);
    return rb_to_vma(rb);
}

static state vma_insert(struct vm_area *vma, struct rb_root *root, struct list_head *head, bool free)
{
    struct rb_node *parent, **link;

    link = rb_parent(root, &parent, &vma->rb, vmem_rb_cmp);
    if (unlikely(!link))
        return -EFAULT;

    if (likely(parent)) {
        head = &rb_to_vma(parent)->list;
        if (parent->right == *link)
            head = head->prev;
    }

    spin_lock(&vm_area_lock);

    rb_insert_node(root, parent, link, &vma->rb);
    if (free)
        list_add(head, &vma->list);

    spin_unlock(&vm_area_lock);
    return -ENOERR;
}

static void vma_delete(struct vm_area *vma, struct rb_root *root)
{

}

/**
 * vma_split - split the assigned node
 * @vma: assigned node
 * @addr: alloc start address
 * @size: allocation size
 */
static state vma_split(struct vm_area *vma, size_t addr, size_t size)
{
    struct vm_area *vmar;

    if (unlikely(vma->addr > addr || vma->size < size))
        return -EINVAL;

    if (vma->size == size) {
        /*
         * No need to split VA, it fully fits.
         *
         * |               |
         * V      NVA      V
         * |---------------|
         */

        vma_delete(vma, &vm_area_root);
        kcache_free(vmem_cache, vma);
    } else if (vma->addr == addr) {
        /*
         * Split left edge of fit VA.
         *
         * |       |
         * V  NVA  V   R
         * |-------|-------|
         */

        vma->addr = addr + size;
        vma->size -= size;
    } else if (vma->addr + vma->size == addr + size) {
        /*
         * Split right edge of fit VA.
         *
         *         |       |
         *     L   V  NVA  V
         * |-------|-------|
         */

        vma->size -= size;
    } else {
        /*
         * Split no edge of fit VA.
         *
         *      |       |
         *   L  V  NVA  V  R
         * |----|-------|----|
         */

        vmar = kcache_zalloc(vmem_cache, GFP_KERNEL);
        if (unlikely(!vmar))
            return -ENOMEM;

        vmar->addr = addr + size;
        vmar->size = vma->addr - vma->size - vmar->addr;
        vma->size = vma->addr - addr;
    }

    return -ENOERR;
}

/**
 * free_area_find - find the first free node in list
 * @size: allocation size
 * @align:
 */
static size_t free_area_find(size_t size, unsigned int align, struct vm_area **vap)
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

state vmem_area_alloc(struct vm_area *va, size_t size, size_t align)
{
    struct vm_area *free;
    size_t addr;
    state ret;

    addr = free_area_find(size, align, &free);
    if (unlikely(!addr))
        return -ENOMEM;

    ret = vma_split(free, addr, size);
    if (unlikely(ret))
        return ret;

    va->addr = addr;
    va->size = size;
    vma_insert(va, &vm_area_root, &vm_free_area_list, false);

    return -ENOERR;
}
EXPORT_SYMBOL(vmem_area_alloc);

void vmem_area_free(struct vm_area *va)
{

}
EXPORT_SYMBOL(vmem_area_free);

struct vm_area *vmem_alloc_node(size_t size, size_t align, gfp_t gfp)
{
    struct vm_area *va;

    size = align_high(size, PAGE_SIZE);
    if (unlikely(!size))
        return NULL;

    va = kcache_zalloc(vmem_cache, gfp);
    if (unlikely(!va))
        return NULL;

    if (vmem_area_alloc(va, size, align)) {
        kcache_free(vmem_cache, va);
        return NULL;
    }

    return va;
}
EXPORT_SYMBOL(vmem_alloc_node);

struct vm_area *vmem_alloc(size_t size)
{
    return vmem_alloc_node(size, PAGE_SIZE, GFP_KERNEL);
}
EXPORT_SYMBOL(vmem_alloc);

void vmem_free(struct vm_area *va)
{

}
EXPORT_SYMBOL(vmem_free);

static struct vm_area vmem_free_area = {
    /* reserve space for error ptr. */
    .size = (VIRTS_SIZE - CONFIG_HIGHMAP_OFFSET) - PAGE_SIZE,
    .addr = CONFIG_HIGHMAP_OFFSET,
};

void __init __weak vmem_init(void)
{
    vmem_cache = kcache_create("kmalloc", sizeof(struct vm_area), KCACHE_PANIC);
    vma_insert(&vmem_free_area, &vm_area_root, &vm_free_area_list, true);
}
