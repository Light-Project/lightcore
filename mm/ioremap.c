/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "ioremap: " fmt

#include <ioremap.h>
#include <kmalloc.h>
#include <mm/vmem.h>
#include <mm/vmap.h>
#include <kernel.h>
#include <rbtree.h>
#include <refcount.h>
#include <export.h>
#include <printk.h>

static SPIN_LOCK(ioremap_lock);
static RB_ROOT(ioremap_root);
static struct kcache *ioremap_cache;

struct ioremap_node {
    struct rb_node rb;
    struct vm_area vm_area;
    phys_addr_t addr;
    unsigned int count;
};

#define rb_to_ioremap(io) \
    container_of(io, struct ioremap_node, rb)

#define rb_to_ioremap_safe(io) \
    container_of_safe(io, struct ioremap_node, rb)

#define vm_to_ioremap(io) \
    container_of(io, struct ioremap_node, vm_area)

struct ioremap_find {
    phys_addr_t addr;
    size_t size;
};

static long ioremap_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    struct ioremap_node *node_a = rb_to_ioremap(rba);
    struct ioremap_node *node_b = rb_to_ioremap(rbb);
    long diff;

    diff = node_a->addr - node_b->addr;
    if (diff)
        return diff;

    return node_a->vm_area.size - node_b->vm_area.size;
}

static long ioremap_rb_find(const struct rb_node *rb, const void *key)
{
    struct ioremap_node *node = rb_to_ioremap(rb);
    const struct ioremap_find *cmp = key;
    long diff;

    diff = cmp->addr - node->addr;
    if (diff)
        return diff;

    return cmp->size <= node->vm_area.size ? 0 : ~0L;
}

static struct ioremap_node *phys_addr_find(phys_addr_t start, phys_addr_t end)
{
    struct rb_node *rb;
    rb = rb_find(&ioremap_root, &(struct ioremap_find){start, end - start}, ioremap_rb_find);
    return rb_to_ioremap_safe(rb);
}

static struct ioremap_node *ioremap_alloc(phys_addr_t pa, size_t size, gvm_t flags)
{
    struct ioremap_node *node;
    state ret;

    node = kcache_zalloc(ioremap_cache, GFP_KERNEL);
    if (unlikely(!node))
        return NULL;

    ret = vmem_area_alloc(&node->vm_area, size, PAGE_SIZE);
    if (unlikely(ret))
        goto error_free;

    ret = vmap_range(&init_mm, pa, node->vm_area.addr, node->vm_area.size, flags, PGDIR_SHIFT);
    if (unlikely(ret))
        goto error_vmfree;

    node->addr = pa;
    rb_insert(&ioremap_root, &node->rb, ioremap_rb_cmp);
    return node;

error_vmfree:
    vmem_area_free(&node->vm_area);
error_free:
    kfree(node);
    return NULL;
}

void *ioremap_node(phys_addr_t pa, size_t size, gvm_t flags)
{
    struct ioremap_node *node;
    phys_addr_t offset;

    if (!size)
        return NULL;

	/* page-align mappings */
    offset = pa & ~PAGE_MASK;
    pa &= PAGE_MASK;
    size = align_high(size, PAGE_SIZE);

    spin_lock(&ioremap_lock);

    node = phys_addr_find(pa, pa + size);
    if (node)
        goto done;

    node = ioremap_alloc(pa, size, flags);
    if (unlikely(!node))
        goto error;

done:
    node->count++;
    spin_unlock(&ioremap_lock);
    return (void *)(node->vm_area.addr + offset);

error:
    spin_unlock(&ioremap_lock);
    return NULL;
}
EXPORT_SYMBOL(ioremap_node);

void *ioremap(phys_addr_t pa, size_t size)
{
#ifdef CONFIG_ARCH_HAS_IOMAP
    if (pa >= CONFIG_RAM_BASE && pa < CONFIG_HIGHMEM_OFFSET)
        return pa_to_io(pa);
#endif
    return ioremap_node(pa, size, GVM_NOCACHE);
}
EXPORT_SYMBOL(ioremap);

void *ioremap_wc(phys_addr_t pa, size_t size)
{
    return ioremap_node(pa, size, GVM_WCOMBINED);
}
EXPORT_SYMBOL(ioremap_wc);

void *ioremap_wt(phys_addr_t pa, size_t size)
{
    return ioremap_node(pa, size, GVM_WTHROUGH);
}
EXPORT_SYMBOL(ioremap_wt);

void *ioremap_uc(phys_addr_t pa, size_t size)
{
    if (pa >= CONFIG_RAM_BASE && pa < CONFIG_HIGHMEM_OFFSET)
        return pa_to_va(pa);

    return ioremap_node(pa, size, 0);
}
EXPORT_SYMBOL(ioremap_uc);

void iounmap(void *block)
{
    size_t addr = (size_t)block;
    struct ioremap_node *node;
    struct vm_area *vm;

    if (addr < CONFIG_HIGHMAP_OFFSET)
        return;

    return;

    vm = vmem_area_find(addr);
    if (!vm) {
        pr_crit("abort iounmap %p\n", block);
        return;
    }

    node = vm_to_ioremap(vm);
    if (!--node->count)
        return;

    vmem_area_free(vm);
    rb_delete(&ioremap_root, &node->rb);
    kcache_free(ioremap_cache, vm);
}
EXPORT_SYMBOL(iounmap);

void __init ioremap_init(void)
{
    ioremap_cache = kcache_create("ioremap",
        sizeof(struct ioremap_node), KCACHE_PANIC);
}
