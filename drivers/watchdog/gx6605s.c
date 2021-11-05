/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-wdt"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>
#include <driver/watchdog/gx6605s.h>
#include <printk.h>
#include <asm/io.h>

struct gx6605s_device {
    struct watchdog_device watchdog;
    void *base;
    spinlock_t lock;
};

#define wdg_to_gx6605s(wdg) \
    container_of(wdg, struct gx6605s_device, watchdog)

static __always_inline uint32_t
gx6605s_read(struct gx6605s_device *gdev, unsigned int reg)
{
    return readl(gdev->base + reg);
}

static __always_inline void
gx6605s_write(struct gx6605s_device *gdev, unsigned int reg, uint32_t val)
{
    writel(gdev->base + reg, val);
}

static state gx6605s_wdt_start(struct watchdog_device *wdev)
{
    struct gx6605s_device *gdev = wdg_to_gx6605s(wdev);
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_WDT_CTRL);
    val |= GX6605S_WDT_CTRL_ENABLE | GX6605S_WDT_CTRL_RESET;
    gx6605s_write(gdev, GX6605S_WDT_CTRL, val);

    return -ENOERR;
}

static state gx6605s_wdt_stop(struct watchdog_device *wdev)
{
    struct gx6605s_device *gdev = wdg_to_gx6605s(wdev);
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_WDT_CTRL);
    val &= ~(GX6605S_WDT_CTRL_ENABLE | GX6605S_WDT_CTRL_RESET);
    gx6605s_write(gdev, GX6605S_WDT_CTRL, val);

    return -ENOERR;
}

static state gx6605s_wdt_feed(struct watchdog_device *wdev)
{
    struct gx6605s_device *gdev = wdg_to_gx6605s(wdev);
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_WDT_WSR) & 0xffff0000;
    gx6605s_write(gdev, GX6605S_WDT_CTRL, val | 0x5555);
    gx6605s_write(gdev, GX6605S_WDT_CTRL, val | 0xaaaa);

    return -ENOERR;
}

static state gx6605s_wdt_timeout_set(struct watchdog_device *wdev, unsigned int secs)
{
    struct gx6605s_device *gdev = wdg_to_gx6605s(wdev);
    uint32_t val;

    gx6605s_wdt_stop(wdev);

    val = gx6605s_read(gdev, GX6605S_WDT_WSR) & 0xffff0000;
    val |= 0x10000 - (1000 * secs); // change to ms
    gx6605s_write(gdev, GX6605S_WDT_CTRL, val);

    gx6605s_wdt_start(wdev);

    return -ENOERR;
}

static struct watchdog_ops gx6605s_wdt_ops = {
    .start = gx6605s_wdt_start,
    .stop = gx6605s_wdt_stop,
    .feed = gx6605s_wdt_feed,
    .timeout_set = gx6605s_wdt_timeout_set,
};

static state gx6605s_wdt_probe(struct platform_device *pdev, void *pdata)
{
    struct gx6605s_device *gdev;
    resource_size_t start, size;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -EINVAL;

    gdev = dev_kzalloc(&pdev->dev, sizeof(*gdev), GFP_KERNEL);
    if (gdev)
        return -ENOERR;
    platform_set_devdata(pdev, gdev);

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    gdev->base = dev_ioremap(&pdev->dev, start, size);
    if (!gdev->base)
        return -ENOMEM;

    gdev->watchdog.dev = &pdev->dev;
    gdev->watchdog.ops = &gx6605s_wdt_ops;
    return watchdog_register(&gdev->watchdog);
}

static struct dt_device_id gx6605s_wdt_ids[] = {
    { .compatible = "csky,gx6605s-wdt" },
    { }, /* NULL */
};

static struct platform_driver gx6605s_wdt_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_wdt_ids,
    .probe = gx6605s_wdt_probe,
};

static state gx6605s_wdt_init(void)
{
    return platform_driver_register(&gx6605s_wdt_driver);
}
driver_initcall(gx6605s_wdt_init);
