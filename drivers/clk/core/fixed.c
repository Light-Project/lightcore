/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "fixed-clock"

#include <initcall.h>
#include <driver/platform.h>
#include <driver/clk.h>

struct fixed_clk_device {
    struct clk_device clk;
    uint32_t freq;
};

#define clk_to_fixed(cdev) \
    container_of(cdev, struct fixed_clk_device, clk)

static state fixed_clk_get_freq(struct clk_device *cdev, unsigned int channel,
                                uint64_t parent, uint64_t *rate)
{
    if (channel)
        return -EIO;

    *rate = clk_to_fixed(cdev)->freq;
    return -ENOERR;
}

static struct clk_ops fixed_clk_ops = {
    .rate_get = fixed_clk_get_freq,
};

static state fixed_clk_probe(struct platform_device *pdev, void *pdata)
{
    struct fixed_clk_device *fixed;
    uint32_t freq;

    if (dt_attribute_read_u32(pdev->dt_node, "clock-frequency", &freq))
        return -ENODEV;

    fixed = dev_kzalloc(&pdev->dev, sizeof(*fixed), GFP_KERNEL);
    if (!fixed)
        return -ENOMEM;
    platform_set_devdata(pdev, fixed);

    fixed->freq = freq;
    fixed->clk.ops = &fixed_clk_ops;
    return clk_register(&fixed->clk);
}

static struct dt_device_id fixed_clk_ids[] = {
    { .compatible = "fixed-clock" },
    { }, /* NULL */
};

static struct platform_driver fixed_clk_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = fixed_clk_ids,
    .probe = fixed_clk_probe,
};

static state fixed_clk_initcall(void)
{
    return platform_driver_register(&fixed_clk_driver);
}
clk_initcall(fixed_clk_initcall);
