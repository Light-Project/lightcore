/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "suniv-intc"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/irqchip/suniv.h>
#include <irq.h>
#include <printk.h>

#include <asm/io.h>

struct suniv_device {
    struct irqchip_device irq;
    void *base;
};

static __always_inline uint32_t
suniv_readl(struct suniv_device *sdev, int reg)
{
    return readl(sdev->base + reg);
}

static __always_inline void
suniv_writel(struct suniv_device *sdev, int reg, uint32_t val)
{
    writel(sdev->base + reg, val);
}

static void suniv_handle(struct regs *regs)
{
    uint32_t vector;

    irq_handle(vector);
}

static state suniv_probe(struct platform_device *pdev, void *pdata)
{
    struct suniv_device *sdev;
    resource_size_t start, size;
    uint32_t val;

    sdev = dev_kzalloc(&pdev->dev, sizeof(*sdev), GFP_KERNEL);
    if (!sdev)
        return -ENOMEM;

    val = platform_resource_type(pdev, 0);
    if (val != RESOURCE_MMIO)
        return -ENODEV;

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    sdev->mmio = ioremap(start, size);
    if (!sdev->mmio)
        return -ENOMEM;

    arch_irq_set(suniv_handle);
    return irqchip_register(&sdev->irq);
}

static struct dt_device_id suniv_ids[] = {
    { .compatible = "allwinner,suniv-intc" },
    { }, /* NULL */
};

static struct platform_driver suniv_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = suniv_ids,
    .probe = suniv_probe,
};

static state suniv_init(void)
{
    platform_driver_register();
}
driver_initcall(suniv_init);
