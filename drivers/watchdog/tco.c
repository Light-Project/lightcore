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

#define wdg_to_tco(wdg) \
    container_of(wdg, struct tco_device, watchdog)

static state tco_start(struct watchdog_device *wdev)
{

    return -ENOERR;
}

static state tco_stop(struct watchdog_device *wdev)
{

    return -ENOERR;
}

static state tco_feed(struct watchdog_device *wdev)
{

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

    switch (tco->version) {
        case 4 ... 6:
            outw(tco->base + TCO_SYS1, 0x0008);
            outw(tco->base + TCO_SYS2, 0x0002);
            break;
        case 3:
            outl(tco->base + TCO_SYS1, 0x00020008);
            break;
        case 1 ... 2: default:
            outw(tco->base + TCO_SYS1, 0x0008);
            outw(tco->base + TCO_SYS2, 0x0002);
            outw(tco->base + TCO_SYS2, 0x0004);
    };

    tco->base = platform_resource_start(pdev, 0);
    tco->watchdog.ops = &tco_ops;
    return watchdog_register(&tco->watchdog);
}

static struct platform_driver tco_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = tco_probe,
};

static state tco_init(void)
{
    return platform_driver_register(&tco_driver);
}
driver_initcall(tco_init);
