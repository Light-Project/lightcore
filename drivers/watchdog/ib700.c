/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ib700wdt"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <ioops.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>
#include <driver/watchdog/ib700.h>
#include <export.h>

struct ib700wdt_device {
    struct watchdog_device watchdog;
    unsigned int timeout;
    resource_size_t base;
};

#define watchdog_to_ib700wdt(wdev) \
    container_of(wdev, struct ib700wdt_device, watchdog)

static state ib700wdt_start(struct watchdog_device *wdev)
{
    struct ib700wdt_device *idev = watchdog_to_ib700wdt(wdev);
    outb_p(idev->base + IB700_WDT_TIMEOUT, idev->timeout);
    return -ENOERR;
}

static state ib700wdt_stop(struct watchdog_device *wdev)
{
    struct ib700wdt_device *idev = watchdog_to_ib700wdt(wdev);
    outb_p(idev->base + IB700_WDT_CLEAR, 0x5a); /* Any value */
    return -ENOERR;
}

static state ib700wdt_timeout_get(struct watchdog_device *wdev, unsigned int *secs)
{
    struct ib700wdt_device *idev = watchdog_to_ib700wdt(wdev);
    *secs = (15 - idev->timeout) * 2;
    return -ENOERR;
}

static state ib700wdt_timeout_set(struct watchdog_device *wdev, unsigned int secs)
{
    struct ib700wdt_device *idev = watchdog_to_ib700wdt(wdev);
    idev->timeout = 15 - DIV_ROUND_UP(secs, 2);
    return -ENOERR;
}

static struct watchdog_ops ib700wdt_ops = {
    .start = ib700wdt_start,
    .stop = ib700wdt_stop,
    .timeout_get = ib700wdt_timeout_get,
    .timeout_set = ib700wdt_timeout_set,
};

static state ib700wdt_probe(struct platform_device *pdev, const void *pdata)
{
    struct ib700wdt_device *idev;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -EINVAL;

    idev = platform_kzalloc(pdev, sizeof(*idev), GFP_KERNEL);
    if (unlikely(!idev))
        return -ENOMEM;

    idev->base = platform_resource_start(pdev, 0);
    idev->timeout = 30;

    idev->watchdog.dev = &pdev->dev;
    idev->watchdog.ops = &ib700wdt_ops;
    idev->watchdog.timeout_min = 0;
    idev->watchdog.timeout_max = 30;

    return watchdog_register(&idev->watchdog);
}

static const struct dt_device_id ib700wdt_ids[] = {
    { .compatible = "ibase,ib700-wdt" },
    { }, /* NULL */
};

static struct platform_driver ib700wdt_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = ib700wdt_ids,
    .probe = ib700wdt_probe,
};

static state ib700wdt_init(void)
{
    return platform_driver_register(&ib700wdt_driver);
}
driver_initcall(ib700wdt_init);
