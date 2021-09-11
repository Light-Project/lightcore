/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "devres: " fmt

#include <mm.h>
#include <export.h>
#include <device.h>
#include <resource.h>
#include <printk.h>

/**
 * devres_release - add resource to device
 * @dev: device to add
 * @devres: added resource
 */
static inline void devres_add(struct device *dev, struct devres *devres)
{
    list_add_prev(&dev->dev_res, &devres->list);
}

/**
 * devres_release - del resource to device
 * @devres: resources to del
 */
static inline void devres_del(struct devres *devres)
{
    list_del(&devres->list);
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
            break;
        default:
            pr_warn("release %s unknow type\n",
                     devres->name);
    }

    devres_del(devres);
}

/**
 * devres_release - release all resource from device
 * @dev: device to release
 */
void devres_release_all(struct device *dev)
{
    struct devres *devres;

    device_for_each_res(devres, dev) {
        devres_release(devres);
    }
}

/**
 * devres_find - find device resources by address
 * @dev: device to find
 * @addr: resources addr
 */
static struct devres *devres_find(struct device *dev, void *addr)
{
    struct devres *devres;

    device_for_each_res(devres, dev)
        if (devres->addr == addr)
            return devres;

    return NULL;
}

void *dev_kmalloc(struct device *dev, size_t size, gfp_t gfp)
{
    struct devres *devres;
    void *block;

    block = kmalloc(size + sizeof(*devres), gfp);
    if (!block)
        return NULL;

    devres = (void *)((uint8_t *)block + size);
    devres->type = DEVRES_KMEM;
    devres->addr = block;
    devres_add(dev, devres);

    return block;
}
EXPORT_SYMBOL(dev_kmalloc);

void *dev_ioremap(struct device *dev, phys_addr_t addr, size_t size)
{
    struct devres *devres;
    void *block;

    block = ioremap(addr, size);
    if (!block)
        return NULL;

    devres = kmalloc(sizeof(*devres), GFP_KERNEL);
    if (!devres) {
        iounmap(block);
        return NULL;
    }

    devres->type = DEVRES_KMEM;
    devres->addr = block;
    devres_add(dev, devres);

    return block;
}
EXPORT_SYMBOL(dev_ioremap);

void dev_kfree(struct device *dev, void *block)
{
    struct devres *devres = devres_find(dev, block);

    if (!devres) {
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
