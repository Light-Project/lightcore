/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "systick"

#include <initcall.h>
#include <timekeeping.h>
#include <driver/platform.h>
#include <driver/clocksource.h>

#if CONFIG_SYSTICK_FREQ < 34
# define SYSTICK_SHIFT 6
#elif CONFIG_SYSTICK_FREQ < 67
# define SYSTICK_SHIFT 7
#else
# define SYSTICK_SHIFT 8
#endif

static uint64_t systick_read(struct clocksource_device *cdev)
{
    return ticktime;
}

static struct clocksource_ops systick_ops = {
    .read = systick_read,
};

static state systick_probe(struct platform_device *pdev, const void *pdata)
{
    struct clocksource_device *cdev;

    cdev = platform_kzalloc(pdev, sizeof(*cdev), GFP_KERNEL);
    if (!cdev)
        return -ENOMEM;

    cdev->device = &pdev->dev,
    cdev->ops = &systick_ops,
    cdev->mask = BIT_RANGE(31, 0),
    cdev->mult = SYSTICK_NSEC << SYSTICK_SHIFT,
    cdev->shift = SYSTICK_SHIFT,
    platform_set_devdata(pdev, cdev);

    return clocksource_register(cdev);
}

static struct platform_driver systick_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = systick_probe,
};

static state systick_init(void)
{
    struct platform_device *pdev;

    pdev = platform_unified_register(&systick_driver, NULL, 0);
    if (!pdev)
        return -ENOMEM;

    return -ENOERR;
}
clocksource_initcall(systick_init);
