/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "rdrand"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/rng.h>
#include <asm/regs.h>
#include <printk.h>

#define SANITY_CHECK_LOOPS 8
#define SANITY_MIN_CHANGE 5

static state rdrand_read(struct rng_device *rdev, unsigned long *data)
{
    return rand_get(data) ? -ENOERR : -ENXIO;
}

static struct rng_ops rdrand_ops = {
    .read = rdrand_read,
};

static state rdrand_probe(struct platform_device *pdev, const void *pdata)
{
    struct rng_device *rdev;
    unsigned int count, changed = 0;
    unsigned long value, prev;
	bool failure = false;

    for (count = 0; count < SANITY_CHECK_LOOPS; ++count) {
        if (!rand_get(&value)) {
            failure = true;
        }
        if (count && value != prev)
            changed++;
        prev = value;
    }

    if (unlikely(changed < SANITY_MIN_CHANGE))
        failure = true;

    if (unlikely(failure)) {
        platform_err(pdev, "rdrand sanity check failed\n");
        return -ENXIO;
    }

    rdev = platform_kzalloc(pdev, sizeof(*rdev), GFP_KERNEL);
    if (unlikely(!rdev))
        return -ENOMEM;

    rdev->dev = &pdev->dev;
    rdev->ops = &rdrand_ops;
    rdev->rating = DEVICE_RATING_PERFECT;

    return rng_register(rdev);
}

static struct platform_driver rdrand_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = rdrand_probe,
};

static state rdrand_init(void)
{
    struct platform_device *pdev;

    if (!cpuid_support(X86_CPUID_RDRAND))
        return -ENODEV;

    pdev = platform_unified_register(&rdrand_driver, NULL, 0);
    if (!pdev)
        return -ENOMEM;

    return -ENOERR;
}
driver_initcall(rdrand_init);
