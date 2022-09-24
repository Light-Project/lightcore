/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "tsc"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <kclock.h>
#include <driver/platform.h>
#include <driver/clocksource.h>
#include <asm/regs.h>
#include <asm/tsc.h>

static uint64_t tsc_read(struct clocksource_device *cdev)
{
    return tsc_get();
}

static struct clocksource_ops tsc_ops = {
    .read = tsc_read,
};

static state tsc_probe(struct platform_device *pdev, const void *pdata)
{
    struct clocksource_device *cdev;

    cdev = platform_kzalloc(pdev, sizeof(*cdev), GFP_KERNEL);
    if (!cdev)
        return -ENOMEM;

    cdev->flags = CLKSRC_CONTINUOUS  | CLKSRC_VALID_HRES |
                  CLKSRC_MUST_VERIFY | CLKSRC_VERIFY_PERCPU;

    cdev->device = &pdev->dev;
    cdev->ops = &tsc_ops;
    cdev->rating = CLOCK_RATING_DESIRED;
    platform_set_devdata(pdev, cdev);

    kclock_register(
        tsc_get, (uint64_t)tsc_khz * 1000,
        BIT_RANGE_ULL(63, 0)
    );

    return clocksource_config_register(
        cdev, (uint64_t)tsc_khz * 1000,
        BIT_RANGE_ULL(63, 0)
    );
}

static const struct dt_device_id tsc_ids[] = {
    { .compatible = "intel,tsc" },
    { }, /* NULL */
};

static struct platform_driver tsc_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = tsc_ids,
    .probe = tsc_probe,
};

static state tsc_clocksorce_init(void)
{
    return platform_driver_register(&tsc_driver);
}
clocksource_initcall(tsc_clocksorce_init);
