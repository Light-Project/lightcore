/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "intel-tco"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>
#include <driver/watchdog/tco.h>
#include <printk.h>

#include <asm/io.h>

struct tco_device {
    struct watchdog_device watchdog;
    unsigned int version;
    resource_size_t base;
    spinlock_t lock;
};

#define wdg_to_tco(wdg) \
    container_of(wdg, struct tco_device, watchdog)

static state tco_start(struct watchdog_device *wdev)
{
    struct tco_device *tco = wdg_to_tco(wdev);

    spin_lock(&tco->lock);

    spin_unlock(&tco->lock);
    return -ENOERR;
}

static state tco_stop(struct watchdog_device *wdev)
{
    struct tco_device *tco = wdg_to_tco(wdev);

    spin_lock(&tco->lock);

    spin_unlock(&tco->lock);
    return -ENOERR;
}

static state tco_feed(struct watchdog_device *wdev)
{
    struct tco_device *tco = wdg_to_tco(wdev);

    spin_lock(&tco->lock);

    if (tco->version == 1) {
        outw(tco->base + TCO_STS1, 0x08);
        outw(tco->base + TCO_RLD, 0x01);
    } else {
        outb(tco->base + TCO_RLD, 0x01);
    }

    spin_unlock(&tco->lock);
    return -ENOERR;
}

static struct watchdog_ops tco_ops = {
    .start = tco_start,
    .stop = tco_stop,
    .feed = tco_feed,
};

static state tco_probe(struct platform_device *pdev, void *pdata)
{
    struct tco_device *tco;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENODEV;

    tco = dev_kzalloc(&pdev->dev, sizeof(*tco), GFP_KERNEL);
    if (!tco)
        return -ENOMEM;

    tco->version = (size_t)pdata;
    platform_set_devdata(pdev, tco);

    switch (tco->version) {
        case 4 ... 6:
            outw(tco->base + TCO_STS1, 0x0008);
            outw(tco->base + TCO_STS2, 0x0002);
            break;
        case 3:
            outl(tco->base + TCO_STS1, 0x00020008);
            break;
        case 1 ... 2: default:
            outw(tco->base + TCO_STS1, 0x0008);
            outw(tco->base + TCO_STS2, 0x0002);
            outw(tco->base + TCO_STS2, 0x0004);
    };

    tco->base = platform_resource_start(pdev, 0);
    tco->watchdog.ops = &tco_ops;
    return watchdog_register(&tco->watchdog);
}

static struct platform_device_id tco_ids[] = {
    { .name = TCO_MATCH_ID(1), .data = (void *)0x01 },
    { .name = TCO_MATCH_ID(2), .data = (void *)0x02 },
    { .name = TCO_MATCH_ID(3), .data = (void *)0x03 },
    { .name = TCO_MATCH_ID(4), .data = (void *)0x04 },
    { .name = TCO_MATCH_ID(5), .data = (void *)0x05 },
    { .name = TCO_MATCH_ID(6), .data = (void *)0x06 },
    { }, /* NULL */
};

static struct platform_driver tco_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .platform_table = tco_ids,
    .probe = tco_probe,
};

static state tco_init(void)
{
    return platform_driver_register(&tco_driver);
}
driver_initcall(tco_init);
