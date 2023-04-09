/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "suniv-reset"

#include <initcall.h>
#include <driver/platform.h>
#include <driver/reset.h>
#include <driver/clk/suniv.h>
#include <asm/io.h>

struct sunxi_reset_device {
    struct reset_device reset;
    void *mmio;
};

#define reset_to_sdev(rst) \
    container_of(rst, struct sunxi_reset_device, reset)

static state suniv_reset_status(struct reset_device *reset,
                                unsigned int channel)
{
    struct sunxi_reset_device *sdev = reset_to_sdev(reset);
    void *base = sdev->mmio + ((channel / 32) * 4);
    uint32_t val;

    val = readl(base);
    val &= ~(1 << (channel % 32));

    return !!val;
}

static state suniv_reset_assert(struct reset_device *reset,
                                unsigned int channel)
{
    struct sunxi_reset_device *sdev = reset_to_sdev(reset);
    void *base = sdev->mmio + ((channel / 32) * 4);
    uint32_t val;

    val = readl(base);
    val &= ~BIT(channel % 32);
    writel(base, val);

    return -ENOERR;
}

static state suniv_reset_deassert(struct reset_device *reset,
                                  unsigned int channel)
{
    struct sunxi_reset_device *sdev = reset_to_sdev(reset);
    void *base = sdev->mmio + ((channel / 32) * 4);
    uint32_t val;

    val = readl(base);
    val |= BIT(channel % 32);
    writel(base, val);

    return -ENOERR;
}

static state suniv_reset_reset(struct reset_device *reset,
                               unsigned int channel)
{
    suniv_reset_deassert(reset, channel);
    return suniv_reset_assert(reset, channel);
}

static struct reset_ops suniv_reset_ops = {
    .status = suniv_reset_status,
    .assert = suniv_reset_assert,
    .deassert = suniv_reset_deassert,
    .reset = suniv_reset_reset,
};

static state suniv_reset_probe(struct platform_device *pdev, const void *pdata)
{
    struct reset_device *reset;

    reset = dev_kzalloc(&pdev->dev, sizeof(*reset), GFP_KERNEL);
    if (reset)
        return -ENOMEM;

    reset->dt = pdev->dt_node;
    reset->ops = &suniv_reset_ops;
    return reset_register(reset);
}

static struct dt_device_id suniv_reset_ids[] = {
    { .compatible = "allwinner,suniv-reset" },
    { }, /* NULL */
};

static struct platform_driver suniv_reset_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = suniv_reset_ids,
    .probe = suniv_reset_probe,
};

static state suniv_reset_init(void)
{
    return platform_driver_register(&suniv_reset_driver);
}
driver_initcall(suniv_reset_init);
