/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "devres: " fmt

#include <device.h>
#include <string.h>
#include <kmalloc.h>
#include <ioremap.h>
#include <export.h>
#include <printk.h>

/**
 * devres_find - find device resources by address
 * @dev: device to find
 * @addr: resources addr
 */
static struct devres *devres_find(struct device *dev, void *addr)
{
    struct devres *devres;

    device_for_each_res(devres, dev) {
        if (devres->addr == addr)
            return devres;
    }

    return NULL;
}

/**
 * devres_release - release one resource from device
 * @devres: devres to release
 */
void devres_release(struct devres *devres)
{
    switch (devres->type) {
        case DEVRES_KMEM:
            kfree(devres->addr);
            break;
        case DEVRES_IOMAP:
            iounmap(devres->addr);
            kfree(devres);
            break;
        default:
            pr_warn("release %s unknow type\n", devres->name);
    }
}

/**
 * devres_release_all - release all resource from device
 * @dev: device to release
 */
void devres_release_all(struct device *dev)
{
    struct devres *devres, *next;

    list_for_each_entry_safe(devres, next, &dev->devres, list) {
        list_del(&devres->list);
        devres_release(devres);
    }
}

void *dev_kmalloc(struct device *dev, size_t size, gfp_t flags)
{
    struct devres *devres;
    void *block;

    block = kmalloc(size + sizeof(*devres), flags);
    if (unlikely(!block))
        return NULL;

    devres = (void *)((uint8_t *)block + size);
    if (!(flags & GFP_ZERO))
        memset(devres, 0, sizeof(*devres));

    devres->type = DEVRES_KMEM;
    devres->addr = block;
    list_add(&dev->devres, &devres->list);

    return block;
}
EXPORT_SYMBOL(dev_kmalloc);

void *dev_ioremap(struct device *dev, phys_addr_t addr, size_t size)
{
    struct devres *devres;
    void *block;

    devres = kzalloc(sizeof(*devres), GFP_KERNEL);
    if (unlikely(!devres))
        return NULL;

    block = ioremap(addr, size);
    if (unlikely(!block)) {
        kfree(block);
        return NULL;
    }

    devres->type = DEVRES_IOMAP;
    devres->addr = block;
    devres->size = size;
    list_add(&dev->devres, &devres->list);

    return block;
}
EXPORT_SYMBOL(dev_ioremap);

void dev_kfree(struct device *dev, void *block)
{
    struct devres *devres = devres_find(dev, block);

    if (unlikely(!devres)) {
        dev_crit(dev, "resource %p out bounds\n", block);
        return;
    }

    devres_release(devres);
}
EXPORT_SYMBOL(dev_kfree);

void dev_iounmap(struct device *dev, void *block)
{
    dev_kfree(dev, block);
}
EXPORT_SYMBOL(dev_iounmap);
