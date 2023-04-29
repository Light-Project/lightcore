/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "resnode-lib"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <device.h>
#include <kmalloc.h>
#include <vmalloc.h>
#include <ioremap.h>
#include <irq.h>
#include <export.h>

static struct kcache *devres_ioremap_cache;
static struct kcache *devres_irq_cache;

struct devres_malloc {
    struct resnode node;
    size_t offset;
};

struct devres_ioremap {
    struct resnode node;
    void *block;
};

struct devres_irq {
    struct resnode node;
    irqnr_t vector;
};

#define devres_to_reslib(ptr, type) container_of(ptr, struct type, node)
#define devres_to_malloc(ptr)       devres_to_reslib(ptr, devres_malloc)
#define devres_to_ioremap(ptr)      devres_to_reslib(ptr, devres_ioremap)
#define devres_to_irq(ptr)          devres_to_reslib(ptr, devres_irq)

static void release_kmalloc(struct respool *pool, struct resnode *node)
{
    struct devres_malloc *dkma = devres_to_malloc(node);
    void *block = (void *)dkma - dkma->offset;
    kfree(block);
}

static bool find_kmalloc(struct respool *pool, struct resnode *node, const void *data)
{
    struct devres_malloc *dkma = devres_to_malloc(node);
    void *block = (void *)dkma - dkma->offset;

    if (node->release != release_kmalloc)
        return false;

    return data == block;
}

void *dev_kmalloc_align(struct device *dev, size_t size, gfp_t flags, size_t align)
{
    struct devres_malloc *dkma;
    void *block;

    if (unlikely(!size))
        return NULL;

    size = align_high(size, MSIZE);
    block = kmalloc_align(size + sizeof(*dkma), flags, align);
    if (unlikely(!block))
        return NULL;

    dkma = block + size;
    if (!(flags & GFP_ZERO))
        memset(dkma, 0, sizeof(*dkma));

    dkma->offset = size;
    dkma->node.name = "kmalloc";
    dkma->node.release = release_kmalloc;
    respool_insert(&dev->devres, &dkma->node);

    return block;
}
EXPORT_SYMBOL(dev_kmalloc_align);

void *dev_kzalloc_align(struct device *dev, size_t size, gfp_t flags, size_t align)
{
    return dev_kmalloc_align(dev, size, flags | GFP_ZERO, align);
}
EXPORT_SYMBOL(dev_kzalloc_align);

void *dev_kmalloc(struct device *dev, size_t size, gfp_t flags)
{
    return dev_kmalloc_align(dev, size, flags, 0);
}
EXPORT_SYMBOL(dev_kmalloc);

void *dev_kzalloc(struct device *dev, size_t size, gfp_t flags)
{
    return dev_kmalloc_align(dev, size, flags | GFP_ZERO, 0);
}
EXPORT_SYMBOL(dev_kzalloc);

void dev_kfree(struct device *dev, void *block)
{
    struct resnode *node;
    node = respool_find_release(&dev->devres, find_kmalloc, block);
    if (unlikely(!node))
        dev_crit(dev, "illegal kfree %p\n", block);
}
EXPORT_SYMBOL(dev_kfree);

static void release_vmalloc(struct respool *pool, struct resnode *node)
{
    struct devres_malloc *dvma = devres_to_malloc(node);
    void *block = (void *)dvma - dvma->offset;
    vfree(block);
}

static bool find_vmalloc(struct respool *pool, struct resnode *node, const void *data)
{
    struct devres_malloc *dvma = devres_to_malloc(node);
    void *block = (void *)dvma - dvma->offset;

    if (node->release != release_vmalloc)
        return false;

    return data == block;
}

void *dev_vmalloc(struct device *dev, size_t size)
{
    struct devres_malloc *dvma;
    void *block;

    if (unlikely(!size))
        return NULL;

    size = align_high(size, MSIZE);
    block = vmalloc(size + sizeof(*dvma));
    if (unlikely(!block))
        return NULL;

    dvma = block + size;
    memset(dvma, 0, sizeof(*dvma));

    dvma->offset = size;
    dvma->node.name = "vmalloc";
    dvma->node.release = release_vmalloc;
    respool_insert(&dev->devres, &dvma->node);

    return block;
}
EXPORT_SYMBOL(dev_vmalloc);

void dev_vfree(struct device *dev, void *block)
{
    struct resnode *node;
    node = respool_find_release(&dev->devres, find_vmalloc, block);
    if (unlikely(!node))
        dev_crit(dev, "illegal vfree %p\n", block);
}
EXPORT_SYMBOL(dev_vfree);

static void release_ioremap(struct respool *pool, struct resnode *node)
{
    struct devres_ioremap *diomap = devres_to_ioremap(node);
    iounmap(diomap->block);
    kcache_free(devres_ioremap_cache, diomap);
}

static bool find_ioremap(struct respool *pool, struct resnode *node, const void *data)
{
    struct devres_ioremap *diomap = devres_to_ioremap(node);

    if (node->release != release_ioremap)
        return false;

    return data == diomap->block;
}

void *dev_ioremap_node(struct device *dev, phys_addr_t addr, size_t size, gvm_t flags)
{
    struct devres_ioremap *diomap;
    void *block;

    diomap = kcache_zalloc(devres_ioremap_cache, GFP_KERNEL);
    if (unlikely(!diomap))
        return NULL;

    block = ioremap_node(addr, size, flags);
    if (unlikely(!block)) {
        kfree(diomap);
        return NULL;
    }

    diomap->block = block;
    diomap->node.name = "ioremap";
    diomap->node.release = release_ioremap;
    respool_insert(&dev->devres, &diomap->node);

    return block;
}
EXPORT_SYMBOL(dev_ioremap_node);

void *dev_ioremap_wc(struct device *dev, phys_addr_t pa, size_t size)
{
    return dev_ioremap_node(dev, pa, size, GVM_WCOMBINED);
}
EXPORT_SYMBOL(dev_ioremap_wc);

void *dev_ioremap_wt(struct device *dev, phys_addr_t pa, size_t size)
{
    return dev_ioremap_node(dev, pa, size, GVM_WTHROUGH);
}
EXPORT_SYMBOL(dev_ioremap_wt);

void *dev_ioremap(struct device *dev, phys_addr_t pa, size_t size)
{
    return dev_ioremap_node(dev, pa, size, GVM_NOCACHE);
}
EXPORT_SYMBOL(dev_ioremap);

void *dev_ioremap_resource(struct device *dev, struct resource *node)
{
    if (!node || resource_type(node) != RESOURCE_MMIO) {
        dev_crit(dev, "ioremap illegal resource");
        return NULL;
    }
    return dev_ioremap(dev, node->start, node->size);
}
EXPORT_SYMBOL(dev_ioremap_resource);

void dev_iounmap(struct device *dev, void *block)
{
    struct resnode *node;
    node = respool_find_release(&dev->devres, find_ioremap, block);
    if (unlikely(!node))
        dev_crit(dev, "illegal iounmap %p\n", block);
}
EXPORT_SYMBOL(dev_iounmap);

static void release_irq(struct respool *pool, struct resnode *node)
{
    struct devres_irq *dirq = devres_to_irq(node);
    irq_release(dirq->vector);
    kcache_free(devres_irq_cache, dirq);
}

static bool find_irq(struct respool *pool, struct resnode *node, const void *data)
{
    struct devres_irq *dirq = devres_to_irq(node);

    if (node->release != release_irq)
        return false;

    return (irqnr_t)data == dirq->vector;
}

state dev_irq_request(struct device *dev, irqnr_t vector, enum irq_flags flags,
                      irq_handler_t handler, void *data, const char *name)
{
    struct devres_irq *dirq;
    state retval;

    dirq = kcache_zalloc(devres_irq_cache, GFP_KERNEL);
    if (unlikely(!dirq))
        return -ENOMEM;

    retval = irq_request(vector, flags, handler, data, name);
    if (unlikely(retval)) {
        kcache_free(devres_irq_cache, dirq);
        return retval;
    }

    dirq->vector = vector;
    dirq->node.name = "irq";
    dirq->node.release = release_irq;
    respool_insert(&dev->devres, &dirq->node);

    return retval;
}
EXPORT_SYMBOL(dev_irq_request);

void dev_irq_release(struct device *dev, irqnr_t vector)
{
    struct resnode *node;
    node = respool_find_release(&dev->devres, find_irq, (void *)vector);
    if (unlikely(!node))
        dev_crit(dev, "illegal irq release %lu\n", vector);
}
EXPORT_SYMBOL(dev_irq_release);

void __init devres_init(void)
{
    devres_ioremap_cache = KCACHE_CREATE(
        struct devres_ioremap, KCACHE_PANIC
    );
    devres_irq_cache = KCACHE_CREATE(
        struct devres_irq, KCACHE_PANIC
    );
}
