/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "devres"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <device.h>
#include <string.h>
#include <kmalloc.h>
#include <vmalloc.h>
#include <ioremap.h>
#include <irq.h>
#include <export.h>
#include <printk.h>

static struct kcache *devres_cache;

/**
 * devres_find - Find one device resources
 * @dev: device to find
 * @addr: resources addr
 */
struct devres *devres_find(struct device *dev, enum devres_type type, void *addr)
{
    struct devres *devres;

    device_for_each_res(devres, dev)
        if (devres->type == type && devres->addr == addr)
            return devres;

    return NULL;
}
EXPORT_SYMBOL(devres_find);

/**
 * devres_release - Release one resource from device
 * @devres: devres to release
 */
void devres_release(struct devres *devres)
{
    switch (devres->type) {
        case DEVRES_KMEM:
            kfree(devres->addr);
            break;
        case DEVRES_VMEM:
            vfree(devres->addr);
            fallthrough;
        case DEVRES_IOMAP:
            iounmap(devres->addr);
            fallthrough;
        case DEVRES_IRQ:
            irq_release((size_t)devres->addr);
            kcache_free(devres_cache, devres);
            break;
        default:
            pr_warn("release %s unknow type\n", devres->name);
    }
}
EXPORT_SYMBOL(devres_release);

/**
 * devres_release_all - release all resource on device
 * @dev: device to release all
 */
void devres_release_all(struct device *dev)
{
    struct devres *devres, *next;

    list_for_each_entry_safe(devres, next, &dev->devres, list) {
        list_del(&devres->list);
        devres_release(devres);
    }
}
EXPORT_SYMBOL(devres_release_all);

/**
 * dev_kmalloc_align - device managed align kmalloc
 * @dev: device to kmalloc
 * @size: kmalloc size
 * @gfp: kmalloc gfp flags
 */
void *dev_kmalloc_align(struct device *dev, size_t size, gfp_t flags, size_t align)
{
    struct devres *devres;
    void *block;

    if (unlikely(!size))
        return NULL;

    block = kmalloc_align(size + sizeof(*devres), flags, align);
    if (unlikely(!block))
        return NULL;

    devres = (void *)((uint8_t *)block + size);
    if (!(flags & GFP_ZERO))
        memset(devres, 0, sizeof(*devres));

    devres->type = DEVRES_KMEM;
    devres->addr = block;
    list_add_prev(&dev->devres, &devres->list);

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

/**
 * dev_kfree - device managed kfree
 * @dev: device to kfree
 * @block: kfree block
 */
void dev_kfree(struct device *dev, void *block)
{
    struct devres *devres;

    devres = devres_find(dev, DEVRES_KMEM, block);
    if (unlikely(!devres)) {
        dev_crit(dev, "kmem %p illegal release\n", block);
        return;
    }

    devres_release(devres);
}
EXPORT_SYMBOL(dev_kfree);

/**
 * dev_vmalloc - device managed vmalloc
 * @dev: device to vmalloc
 * @size: vmalloc size
 */
void *dev_vmalloc(struct device *dev, size_t size)
{
    struct devres *devres;
    void *block;

    devres = kcache_zalloc(devres_cache, GFP_KERNEL);
    if (unlikely(!devres))
        return NULL;

    block = vmalloc(size);
    if (unlikely(!block)) {
        kfree(devres);
        return NULL;
    }

    devres->type = DEVRES_VMEM;
    devres->addr = block;
    list_add_prev(&dev->devres, &devres->list);

    return block;
}
EXPORT_SYMBOL(dev_vmalloc);

/**
 * dev_vfree - device managed vfree
 * @dev: device to vfree
 * @block: vfree block
 */
void dev_vfree(struct device *dev, void *block)
{
    struct devres *devres;

    devres = devres_find(dev, DEVRES_KMEM, block);
    if (unlikely(!devres)) {
        dev_crit(dev, "vmem %p illegal release\n", block);
        return;
    }

    devres_release(devres);
}
EXPORT_SYMBOL(dev_vfree);

/**
 * dev_ioremap - device managed ioremap_node
 * @dev: device to ioremap_node
 * @addr: ioremap_node address
 * @size: ioremap_node size
 * @flags: ioremap_node flags
 */
void *dev_ioremap_node(struct device *dev, phys_addr_t addr, size_t size, gvm_t flags)
{
    struct devres *devres;
    void *block;

    devres = kcache_zalloc(devres_cache, GFP_KERNEL);
    if (unlikely(!devres))
        return NULL;

    block = ioremap_node(addr, size, flags);
    if (unlikely(!block)) {
        kfree(devres);
        return NULL;
    }

    devres->type = DEVRES_IOMAP;
    devres->addr = block;
    list_add_prev(&dev->devres, &devres->list);

    return block;
}
EXPORT_SYMBOL(dev_ioremap_node);

/**
 * dev_ioremap_wc - device managed ioremap_wc
 * @dev: device to ioremap_wc
 * @addr: ioremap_wc address
 * @size: ioremap_wc size
 */
void *dev_ioremap_wc(struct device *dev, phys_addr_t pa, size_t size)
{
    return dev_ioremap_node(dev, pa, size, GVM_WCOMBINED);
}
EXPORT_SYMBOL(dev_ioremap_wc);

/**
 * dev_ioremap_wt - device managed ioremap_wt
 * @dev: device to ioremap_wt
 * @addr: ioremap_wt address
 * @size: ioremap_wt size
 */
void *dev_ioremap_wt(struct device *dev, phys_addr_t pa, size_t size)
{
    return dev_ioremap_node(dev, pa, size, GVM_WTHROUGH);
}
EXPORT_SYMBOL(dev_ioremap_wt);

/**
 * dev_ioremap - device managed ioremap
 * @dev: device to ioremap
 * @addr: ioremap address
 * @size: ioremap size
 */
void *dev_ioremap(struct device *dev, phys_addr_t pa, size_t size)
{
    return dev_ioremap_node(dev, pa, size, GVM_NOCACHE);
}
EXPORT_SYMBOL(dev_ioremap);

/**
 * dev_ioremap_resource - device managed ioremap resource
 * @dev: device to ioremap
 * @res: resource to be handled
 */
void *dev_ioremap_resource(struct device *dev, struct resource *res)
{
    if (!res || resource_type(res) != RESOURCE_MMIO) {
        dev_crit(dev, "ioremap illegal resource");
        return NULL;
    }
    return dev_ioremap(dev, res->start, res->size);
}
EXPORT_SYMBOL(dev_ioremap_resource);

/**
 * dev_iounmap - device managed iounmap
 * @dev: device to iounmap
 * @block: iounmap block
 */
void dev_iounmap(struct device *dev, void *block)
{
    struct devres *devres;

    devres = devres_find(dev, DEVRES_KMEM, block);
    if (unlikely(!devres)) {
        dev_crit(dev, "iomem %p illegal release\n", block);
        return;
    }

    devres_release(devres);
}
EXPORT_SYMBOL(dev_iounmap);

/**
 * dev_irq_request - device managed irq_request
 * @dev: device to irq_request
 * @vector: irq_request vector
 * @flags: irq_request flags
 * @handler: irq_request handler
 * @data: irq_request data
 * @name: irq_request name
 */
state dev_irq_request(struct device *dev, irqnr_t vector, enum irq_flags flags,
                      irq_handler_t handler, void *data, const char *name)
{
    struct devres *devres;
    state ret;

    devres = kcache_zalloc(devres_cache, GFP_KERNEL);
    if (unlikely(!devres))
        return -ENOMEM;

    ret = irq_request(vector, flags, handler, data, name);
    if (unlikely(ret)) {
        kfree(devres);
        return ret;
    }

    devres->type = DEVRES_IRQ;
    devres->addr = (void *)vector;
    list_add_prev(&dev->devres, &devres->list);

    return ret;
}
EXPORT_SYMBOL(dev_irq_request);

/**
 * dev_irq_release - device managed irq_release
 * @dev: device to irq_release
 * @vector: irq_release vector
 */
void dev_irq_release(struct device *dev, irqnr_t vector)
{
    struct devres *devres;

    devres = devres_find(dev, DEVRES_KMEM, (void *)vector);
    if (unlikely(!devres)) {
        dev_crit(dev, "irq %lu illegal release\n", vector);
        return;
    }

    devres_release(devres);
}
EXPORT_SYMBOL(dev_irq_release);

void __init devres_init(void)
{
    devres_cache = kcache_create(MODULE_NAME,
        sizeof(struct devres), KCACHE_PANIC);
}
