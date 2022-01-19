/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-wdt"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/power.h>
#include <driver/watchdog.h>
#include <driver/watchdog/gx6605s.h>
#include <printk.h>
#include <asm/delay.h>
#include <asm/io.h>

struct gx6605s_device {
    struct watchdog_device watchdog;
    struct power_device power;
    void *base;
    spinlock_t lock;
};

#define watchdog_to_gx6605s(wdev) \
    container_of(wdev, struct gx6605s_device, watchdog)

#define power_to_gx6605s(pdev) \
    container_of(pdev, struct gx6605s_device, power)

#define GX6605S_WDT_FREQ    1000

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
    struct gx6605s_device *gdev = watchdog_to_gx6605s(wdev);
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_WDT_CTRL);
    val |= GX6605S_WDT_CTRL_ENABLE | GX6605S_WDT_CTRL_RESET;
    gx6605s_write(gdev, GX6605S_WDT_CTRL, val);

    return -ENOERR;
}

static state gx6605s_wdt_stop(struct watchdog_device *wdev)
{
    struct gx6605s_device *gdev = watchdog_to_gx6605s(wdev);
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_WDT_CTRL);
    val &= ~(GX6605S_WDT_CTRL_ENABLE | GX6605S_WDT_CTRL_RESET);
    gx6605s_write(gdev, GX6605S_WDT_CTRL, val);

    return -ENOERR;
}

static state gx6605s_wdt_feed(struct watchdog_device *wdev)
{
    struct gx6605s_device *gdev = watchdog_to_gx6605s(wdev);
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_WDT_WSR) & 0xffff0000;
    gx6605s_write(gdev, GX6605S_WDT_WSR, val | GX6605S_WDT_WSR_MASK1);
    gx6605s_write(gdev, GX6605S_WDT_WSR, val | GX6605S_WDT_WSR_MASK2);

    return -ENOERR;
}

static state gx6605s_wdt_timeout_set(struct watchdog_device *wdev, unsigned int secs)
{
    struct gx6605s_device *gdev = watchdog_to_gx6605s(wdev);
    uint32_t val;

    gx6605s_wdt_stop(wdev);

    val = gx6605s_read(gdev, GX6605S_WDT_MATCH);
    val &= ~GX6605S_WDT_MATCH_TIMEOUT;
    val |= 0x10000 - (GX6605S_WDT_FREQ * secs);
    gx6605s_write(gdev, GX6605S_WDT_MATCH, val);

    gx6605s_wdt_start(wdev);

    return -ENOERR;
}

static void gx6605s_wdt_reset(struct power_device *pdev)
{
    struct gx6605s_device *gdev = power_to_gx6605s(pdev);
    uint32_t val;

    gx6605s_wdt_stop(&gdev->watchdog);

    val = gx6605s_read(gdev, GX6605S_WDT_MATCH);
    val &= ~GX6605S_WDT_MATCH_TIMEOUT;
    val |= 0x10000 - 10;
    gx6605s_write(gdev, GX6605S_WDT_MATCH, val);

    gx6605s_wdt_start(&gdev->watchdog);
    mdelay(50);
}

static struct watchdog_ops gx6605s_wdt_ops = {
    .start = gx6605s_wdt_start,
    .stop = gx6605s_wdt_stop,
    .feed = gx6605s_wdt_feed,
    .timeout_set = gx6605s_wdt_timeout_set,
};

static struct power_ops watchdog_power_ops = {
    .restart = gx6605s_wdt_reset,
};

static void gx6605s_wdt_hw_init(struct platform_device *pdev, unsigned int freq)
{
    struct gx6605s_device *gdev = platform_get_devdata(pdev);
    uint16_t div;

    gx6605s_write(gdev, GX6605S_WDT_CTRL, 0);
    gx6605s_write(gdev, GX6605S_WDT_MATCH, 0);
    gx6605s_write(gdev, GX6605S_WDT_COUNT, 0);
    gx6605s_write(gdev, GX6605S_WDT_WSR, 0);

    /* setup clock div */
    div = 27000000 / freq - 1;
    gx6605s_write(gdev, GX6605S_WDT_MATCH, div << 16);
}

static state gx6605s_wdt_probe(struct platform_device *pdev, const void *pdata)
{
    struct gx6605s_device *gdev;
    resource_size_t start, size;
    state ret;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -EINVAL;

    gdev = dev_kzalloc(&pdev->dev, sizeof(*gdev), GFP_KERNEL);
    if (!gdev)
        return -ENOERR;
    platform_set_devdata(pdev, gdev);

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    gdev->base = dev_ioremap(&pdev->dev, start, size);
    if (!gdev->base)
        return -ENOMEM;

    gx6605s_wdt_hw_init(pdev, GX6605S_WDT_FREQ);

    gdev->power.dev = &pdev->dev;
    gdev->power.ops = &watchdog_power_ops;
    if ((ret = power_register(&gdev->power)))
        return ret;

    gdev->watchdog.dev = &pdev->dev;
    gdev->watchdog.ops = &gx6605s_wdt_ops;
    if ((ret = watchdog_register(&gdev->watchdog)))
        goto err_power;

    return -ENOERR;

err_power:
    power_unregister(&gdev->power);
    return ret;
}

static struct dt_device_id gx6605s_wdt_ids[] = {
    { .compatible = "nationalchip,gx6605s-wdt" },
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
