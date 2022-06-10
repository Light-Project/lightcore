/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <initcall.h>
#include <driver/serio.h>
#include <export.h>
#include <printk.h>

static __always_inline bool
serio_device_match_one(const struct serio_device_id *id,
                       struct serio_device *sdev)
{
    return ((id->type  == SERIO_ANY_ID || id->type  == sdev->id.type)  &&
            (id->extra == SERIO_ANY_ID || id->extra == sdev->id.extra) &&
            (id->id    == SERIO_ANY_ID || id->id    == sdev->id.id) &&
            (id->proto == SERIO_ANY_ID || id->proto == sdev->id.proto));
}

static inline const struct serio_device_id *
serio_device_match(struct serio_device *sdev,
                   struct serio_driver *sdrv)
{
    const struct serio_device_id *sids = sdrv->id_table;

    while (sids->type && sids->extra) {
        if (serio_device_match_one(sids, sdev))
            return sids;
        ++sids;
    }

    return NULL;
}

static state serio_match(struct device *dev, struct driver *drv)
{
    struct serio_device *sdev = device_to_serio_device(dev);
    struct serio_driver *sdrv = driver_to_serio_driver(drv);
    const struct serio_device_id *sid;

    sid = serio_device_match(sdev, sdrv);
    if (!sid)
        return -ENODEV;

    device_set_pdata(dev, (void *)sid->data);
    return -ENOERR;
}

static state serio_probe(struct device *dev)
{
    struct serio_device *sdev = device_to_serio_device(dev);
    struct serio_driver *sdrv = driver_to_serio_driver(dev->driver);
    const void *pdata;

    if (unlikely(!sdrv->probe)) {
        pr_warn("probe not supported\n");
        return -ENXIO;
    }

    pdata = device_get_pdata(dev);
    return sdrv->probe(sdev, pdata);
}

static state serio_remove(struct device *dev)
{
    struct serio_device *sdev = device_to_serio_device(dev);
    struct serio_driver *sdrv = driver_to_serio_driver(dev->driver);

    if (!sdrv->remove)
        return -ENXIO;

    sdrv->remove(sdev);
    return -ENOERR;
}

static state serio_shutdown(struct device *dev)
{
    struct serio_device *sdev = device_to_serio_device(dev);
    struct serio_driver *sdrv = driver_to_serio_driver(dev->driver);

    if (!sdrv->shutdown)
        return -ENXIO;

    sdrv->shutdown(sdev);
    return -ENOERR;
}

struct bus_type serio_bus = {
    .name = "serio",
    .match = serio_match,
    .probe = serio_probe,
    .remove = serio_remove,
    .shutdown = serio_shutdown,
};

irqreturn_t serio_interrupt(struct serio_host *host, uint32_t data, const void *pdata)
{
    struct serio_device *sdev = &host->port;
    struct serio_driver *sdrv;
    struct driver *drv;
    irqreturn_t retval = IRQ_RET_NONE;
    irqflags_t irqflags;

    spin_lock_irqsave(&host->lock, &irqflags);

    if ((drv = sdev->dev.driver) && /* is it probe */
        (sdrv = driver_to_serio_driver(drv))->interrupt)
        retval = sdrv->interrupt(sdev, data, pdata);

    spin_unlock_irqrestore(&host->lock, &irqflags);
    return retval;
}

state serio_device_register(struct serio_device *sdev)
{
    sdev->dev.bus = &serio_bus;
    return device_register(&sdev->dev);
}

void serio_device_unregister(struct serio_device *sdev)
{
    device_unregister(&sdev->dev);
}

state serio_host_register(struct serio_host *host)
{
    return serio_device_register(&host->port);
}
EXPORT_SYMBOL(serio_host_register);

void serio_host_unregister(struct serio_host *host)
{
    return serio_device_unregister(&host->port);
}
EXPORT_SYMBOL(serio_host_unregister);

state serio_driver_register(struct serio_driver *sdrv)
{
    sdrv->driver.bus = &serio_bus;
    return driver_register(&sdrv->driver);
}
EXPORT_SYMBOL(serio_driver_register);

void serio_driver_unregister(struct serio_driver *sdrv)
{
    driver_unregister(&sdrv->driver);
}
EXPORT_SYMBOL(serio_driver_unregister);

static __init state serio_bus_init(void)
{
    return bus_register(&serio_bus);
}
framework_initcall(serio_bus_init);
