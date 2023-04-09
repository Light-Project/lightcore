/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "ioremap"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <ioremap.h>
#include <memory.h>
#include <kernel.h>
#include <rbtree.h>
#include <kmalloc.h>
#include <mm/vmem.h>
#include <mm/vmap.h>
#include <printk.h>
#include <crash.h>
#include <export.h>

static SPIN_LOCK(ioremap_lock);
static RB_ROOT(ioremap_root);
static struct kcache *ioremap_cache;

struct ioremap_node {
    struct rb_node rb;
    struct vmem_area *vma;
    phys_addr_t addr;
    gvm_t flags;
    unsigned int count;
};

#define rb_to_ioremap(io) \
    rb_entry(io, struct ioremap_node, rb)

#define rb_to_ioremap_safe(io) \
    rb_entry_safe(io, struct ioremap_node, rb)

struct ioremap_find {
    phys_addr_t addr;
    size_t size;
    gvm_t flags;
};

static long ioremap_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    const struct ioremap_node *node_a = rb_to_ioremap(rba);
    const struct ioremap_node *node_b = rb_to_ioremap(rbb);

    if (node_a->flags != node_b->flags)
        return node_a->flags < node_b->flags ? -1 : 1;

    if (node_a->addr != node_b->addr)
        return node_a->addr < node_b->addr ? -1 : 1;

    if (node_a->vma->size != node_b->vma->size)
        return node_a->vma->size < node_b->vma->size ? -1 : 1;

    BUG();
    return 0;
}

static long ioremap_rb_find(const struct rb_node *rb, const void *key)
{
    const struct ioremap_node *node = rb_to_ioremap(rb);
    const struct ioremap_find *cmp = key;
    phys_addr_t node_end, cmp_end;

    node_end = node->addr + node->vma->size;
    cmp_end = cmp->addr + cmp->size;

    if (node->flags == cmp->flags &&
        node->addr <= cmp->addr && cmp_end <= node_end)
        return 0;

    if (node->flags != cmp->flags)
        return node->flags < cmp->flags ? -1 : 1;

    if (node->addr != cmp->addr)
        return node->addr < cmp->addr ? -1 : 1;

    if (node->vma->size != cmp->size)
        return node->vma->size < cmp->size ? -1 : 1;

    return LONG_MIN;
}

static struct ioremap_node *phys_addr_find(phys_addr_t start, size_t size, gvm_t flags)
{
    struct rb_node *rb;
    rb = rb_find(&ioremap_root, &(struct ioremap_find){start, size, flags}, ioremap_rb_find);
    return rb_to_ioremap_safe(rb);
}

static struct ioremap_node *ioremap_alloc(phys_addr_t phys, size_t size, gvm_t flags)
{
    struct ioremap_node *node;
    state ret;

    node = kcache_zalloc(ioremap_cache, GFP_KERNEL);
    if (unlikely(!node))
        return NULL;

    node->vma = vmem_alloc(size);
    if (unlikely(!node->vma))
        goto error_free;

    ret = vmap_range(&init_mm, phys, node->vma->addr, node->vma->size, flags, PGDIR_SHIFT);
    if (unlikely(ret))
        goto error_vmfree;

    node->addr = phys;
    node->flags = flags;
    node->vma->pdata = node;
    rb_insert(&ioremap_root, &node->rb, ioremap_rb_cmp);

    return node;

error_vmfree:
    vmem_free(node->vma);
error_free:
    kcache_free(ioremap_cache, node);
    return NULL;
}

void *ioremap_node(phys_addr_t phys, size_t size, gvm_t flags)
{
    struct ioremap_node *node;
    phys_addr_t offset, last_addr;

    /* Don't allow wraparound or zero size */
    last_addr = phys + size - 1;
    if (unlikely(!size || last_addr < phys))
        return NULL;

    /* page-align mappings */
    offset = phys & ~PAGE_MASK;
    align_low_adj(phys, PAGE_SIZE);
    size = page_align(last_addr + 1) - phys;

    spin_lock(&ioremap_lock);

    node = phys_addr_find(phys, size, flags);
    if (node)
        goto done;

    node = ioremap_alloc(phys, size, flags);
    if (unlikely(!node))
        goto error;

done:
    node->count++;
    spin_unlock(&ioremap_lock);
    return (void *)(node->vma->addr + offset);

error:
    spin_unlock(&ioremap_lock);
    return NULL;
}
EXPORT_SYMBOL(ioremap_node);

void *ioremap(phys_addr_t phys, size_t size)
{
#ifdef CONFIG_ARCH_HAS_IOMAP
    if (phys >= CONFIG_RAM_BASE && phys < CONFIG_HIGHMEM_OFFSET)
        return pa_to_io(phys);
#endif
    return ioremap_node(phys, size, GVM_NOCACHE);
}
EXPORT_SYMBOL(ioremap);

void *ioremap_uc(phys_addr_t phys, size_t size)
{
    if (phys >= CONFIG_RAM_BASE && phys < CONFIG_HIGHMEM_OFFSET)
        return pa_to_va(phys);

    return ioremap_node(phys, size, 0);
}
EXPORT_SYMBOL(ioremap_uc);

void *ioremap_wc(phys_addr_t phys, size_t size)
{
    return ioremap_node(phys, size, GVM_WCOMBINED);
}
EXPORT_SYMBOL(ioremap_wc);

void *ioremap_wt(phys_addr_t phys, size_t size)
{
    return ioremap_node(phys, size, GVM_WTHROUGH);
}
EXPORT_SYMBOL(ioremap_wt);

void iounmap(void *block)
{
    uintptr_t addr = (uintptr_t)block;
    struct ioremap_node *node;
    struct vmem_area *vma;

    if (unlikely(!addr))
        return;

    if (WARN_ON(addr < CONFIG_HIGHMAP_OFFSET))
        return;

    spin_lock(&ioremap_lock);
    align_low_adj(addr, PAGE_SIZE);

    vma = vmem_find(addr);
    if (!vma) {
        pr_crit("abort iounmap %p\n", block);
        goto finish;
    }

    node = vma->pdata;
    if (--node->count)
        goto finish;

    rb_delete(&ioremap_root, &node->rb);
    spin_unlock(&ioremap_lock);

    vunmap_range(&init_mm, vma->addr, vma->size);
    vmem_free(vma);
    kcache_free(ioremap_cache, node);

    return;

finish:
    spin_unlock(&ioremap_lock);
}
EXPORT_SYMBOL(iounmap);

void __init ioremap_init(void)
{
    ioremap_cache = kcache_create("ioremap",
        sizeof(struct ioremap_node), KCACHE_PANIC);
}
