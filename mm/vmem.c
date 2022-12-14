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
#include <panic.h>
#include <asm/page.h>

#define VMALLOC_START   (CONFIG_HIGHMAP_OFFSET)
#define VMALLOC_END     (VMALLOC_START + CONFIG_HIGHMAP_SIZE)

static RB_ROOT(vm_area_root);
static LIST_HEAD(vm_area_list);
static SPIN_LOCK(vm_area_lock);

static RB_ROOT(vm_free_area_root);
static LIST_HEAD(vm_free_area_list);
static SPIN_LOCK(vm_free_area_lock);

static struct kcache *vmem_cache;
static bool __read_mostly vmem_initialized;

static long vmem_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    const struct vmem_area *vma_a = rb_to_vma(rba);
    const struct vmem_area *vma_b = rb_to_vma(rbb);
    return vma_a->addr < vma_b->addr ? -1 : 1;
}

static long vmem_rb_find(const struct rb_node *rb, const void *key)
{
    const struct vmem_area *vma = rb_to_vma(rb);
    if (vma->addr == (uintptr_t)key) return 0;
    return vma->addr < (uintptr_t)key ? -1 : 1;
}

struct vmem_area *vmem_area_find(size_t addr)
{
    struct rb_node *rb;
    spin_lock(&vm_area_lock);
    rb = rb_find(&vm_area_root, (void *)addr, vmem_rb_find);
    spin_unlock(&vm_area_lock);
    return rb_to_vma_safe(rb);
}
EXPORT_SYMBOL(vmem_area_find);

static struct list_head *vma_next_sibling(struct rb_node *parent, struct rb_node **link)
{
    struct list_head *list;

    if (unlikely(!parent))
        return NULL;

    list = &rb_to_vma(parent)->list;
    return link == &parent->left ? list : list->next;
}

static void vma_link(struct vmem_area *vma, struct rb_root *root, struct list_head *head,
                     struct rb_node *parent, struct rb_node **link)
{
    if (likely(parent)) {
        head = &rb_to_vma(parent)->list;
        if (parent->left == *link)
            head = head->prev;
    }

    rb_insert_node(root, parent, link, &vma->rb);
    list_add(head, &vma->list);
}

static state vma_insert(struct vmem_area *vma, struct rb_root *root, struct list_head *head)
{
    struct rb_node *parent, **link;

    link = rb_parent(root, &parent, &vma->rb, vmem_rb_cmp, NULL);
    if (unlikely(!link))
        return -EFAULT;

    vma_link(vma, root, head, parent, link);
    return -ENOERR;
}

static void vma_delete(struct vmem_area *vma, struct rb_root *root)
{
    if (WARN_ON(RB_EMPTY_NODE(&vma->rb)))
        return;

    rb_delete(root, &vma->rb);
    list_del(&vma->list);
    RB_CLEAR_NODE(&vma->rb);
}

/**
 * vma_split - split the assigned node
 * @vma: assigned node
 * @addr: alloc start address
 * @size: allocation size
 */
static state vma_split(struct vmem_area *vma, size_t addr, size_t size,
                       struct rb_root *root, struct list_head *head)
{
    struct vmem_area *vmar;

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

        vmar = kcache_alloc(vmem_cache, GFP_KERNEL);
        if (unlikely(!vmar))
            return -ENOMEM;

        vma->size = vma->addr - addr;
        vmar->addr = addr + size;
        vmar->size = vma->addr - vma->size - vmar->addr;
        vma_insert(vma, root, head);
    }

    return -ENOERR;
}

static state vma_merge(struct vmem_area *vma, struct rb_root *root, struct list_head *head)
{
    struct rb_node *parent, **link;
    struct list_head *next;
    struct vmem_area *sibling;
    bool merged = false;

    link = rb_parent(root, &parent, &vma->rb, vmem_rb_cmp, NULL);
    if (unlikely(!link))
        return -EFAULT;

    next = vma_next_sibling(parent, link);
    if (unlikely(!next))
        goto insert;

    /*
     * start            end
     * |                |
     * |<------VA------>|<-----Next----->|
     *                  |                |
     *                  start            end
     */
    if (unlikely(next != head)) {
        sibling = list_to_vma(next);
        if (sibling->addr == vma->addr + vma->size) {
            sibling->addr = vma->addr;
            sibling->size += vma->size;
            vma = sibling;
            merged = true;
        }
    }

    /*
     * start            end
     * |                |
     * |<-----Prev----->|<------VA------>|
     *                  |                |
     *                  start            end
     */
    if (unlikely(next->prev != head)) {
        sibling = list_to_vma(next->prev);
        if (sibling->addr + sibling->size == vma->addr) {
            sibling->size += vma->size;
            if (merged)
                vma_delete(vma, root);
            vma = sibling;
            merged = true;
        }
    }

insert:
    if (!merged) {
        sibling = kcache_alloc(vmem_cache, GFP_KERNEL);
        if (BUG_ON(!sibling))
            return -ENOMEM;
        *sibling = *vma;
        vma_link(sibling, root, head, parent, link);
    }

    return -ENOERR;
}

static size_t vma_free_find(size_t size, size_t align, struct vmem_area **vma, uintptr_t vstart)
{
    struct vmem_area *va;

    /* traversing free nodes */
    list_for_each_entry(va, &vm_free_area_list, list) {
        size_t align_start, start = va->addr;

        align_start = align_high(start, align);

        /* Prevent overflow caused by too large address. */
        if (align_start + size < align_start || align_start < start)
            continue;

        if (align_start + size <= start + va->size) {
            *vma = va;
            return align_start;
        }
    }

    return 0;
}

static state vma_free_alloc(size_t *raddr, size_t size, size_t align, uintptr_t vstart, uintptr_t vend)
{
    struct vmem_area *free;
    size_t addr;
    state ret;

    addr = vma_free_find(size, align, &free, vstart);
    if (unlikely(!addr))
        return -ENOMEM;

    ret = vma_split(free, addr, size, &vm_free_area_root, &vm_free_area_list);
    if (unlikely(ret))
        return ret;

    *raddr = addr;
    return -ENOERR;
}

state vmem_area_alloc_node(struct vmem_area *vma, size_t size, size_t align, uintptr_t vstart, uintptr_t vend)
{
    uintptr_t addr;
    state ret;

    if (BUG_ON(!vmem_initialized))
        return -EBUSY;

    if (!vma || !size || !is_power_of_2(align))
        return -EINVAL;

    spin_lock(&vm_free_area_lock);
    ret = vma_free_alloc(&addr, size, align, vstart, vend);
    spin_unlock(&vm_free_area_lock);

    if (ret)
        return ret;

    vma->addr = addr;
    vma->size = size;

    spin_lock(&vm_area_lock);
    vma_insert(vma, &vm_area_root, &vm_area_list);
    spin_unlock(&vm_area_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(vmem_area_alloc_node);

state vmem_area_alloc_align(struct vmem_area *vma, size_t size, size_t align)
{
    return vmem_area_alloc_node(vma, size, PAGE_SIZE, VMALLOC_START, VMALLOC_END);
}
EXPORT_SYMBOL(vmem_area_alloc_align);

state vmem_area_alloc(struct vmem_area *vma, size_t size)
{
    return vmem_area_alloc_align(vma, size, PAGE_SIZE);
}
EXPORT_SYMBOL(vmem_area_alloc);

void vmem_area_free(struct vmem_area *vma)
{
    if (BUG_ON(!vmem_initialized))
        return;

    spin_lock(&vm_area_lock);
    vma_delete(vma, &vm_area_root);
    spin_unlock(&vm_area_lock);

    spin_lock(&vm_free_area_lock);
    vma_merge(vma, &vm_free_area_root, &vm_free_area_list);
    spin_unlock(&vm_free_area_lock);
}
EXPORT_SYMBOL(vmem_area_free);

struct vmem_area *vmem_alloc_node(size_t size, size_t align, gfp_t gfp, size_t vstart, size_t vend)
{
    struct vmem_area *va;

    if (BUG_ON(!vmem_initialized))
        return NULL;

    size = align_high(size, PAGE_SIZE);
    if (unlikely(!size))
        return NULL;

    va = kcache_zalloc(vmem_cache, gfp);
    if (unlikely(!va))
        return NULL;

    if (vmem_area_alloc_node(va, size, align, vstart, vend)) {
        kcache_free(vmem_cache, va);
        return NULL;
    }

    return va;
}
EXPORT_SYMBOL(vmem_alloc_node);

struct vmem_area *vmem_alloc_align(size_t size, size_t align, gfp_t gfp)
{
    return vmem_alloc_node(size, align, gfp, VMALLOC_START, VMALLOC_END);
}
EXPORT_SYMBOL(vmem_alloc_align);

struct vmem_area *vmem_alloc(size_t size)
{
    return vmem_alloc_align(size, PAGE_SIZE, GFP_KERNEL);
}
EXPORT_SYMBOL(vmem_alloc);

void vmem_free(struct vmem_area *vma)
{
    if (BUG_ON(!vmem_initialized))
        return;

    vmem_area_free(vma);
    kcache_free(vmem_cache, vma);
}
EXPORT_SYMBOL(vmem_free);

static void __init vmem_free_space_init(void)
{
    size_t vmem_start = CONFIG_HIGHMAP_OFFSET;
    size_t vmem_end = vmem_start + CONFIG_HIGHMAP_SIZE;
    struct vmem_area *vma;

    if (vmem_start < vmem_end) {
        vma = kcache_zalloc(vmem_cache, GFP_KERNEL);
        if (WARN_ON(!vma))
            return;
        vma->addr = vmem_start;
        vma->size = vmem_end - vmem_start;
        vma_insert(vma, &vm_free_area_root, &vm_free_area_list);
    }
}

static void __init vmem_populate(void)
{

}

void __init vmem_init(void)
{
    vmem_cache = kcache_create("kmalloc", sizeof(struct vmem_area), KCACHE_PANIC);
    vmem_populate();
    vmem_free_space_init();
    vmem_initialized = true;
}
