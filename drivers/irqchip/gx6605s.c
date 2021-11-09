/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-intc"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <irq.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/irqchip/gx6605s.h>
#include <asm/bitops.h>
#include <asm/regs.h>
#include <asm/regs.h>
#include <asm/io.h>

struct gx6605s_device {
    struct irqchip_device irqchip;
    void *mmio;
};

#define irqchip_to_gdev(irq_device) \
    container_of(irq_device, struct gx6605s_device, irqchip)

#define irq_source_val(count, magic) \
    (((count) << 24) | ((count) << 16) | ((count) << 8) | (count) | (magic))

static struct gx6605s_device *root;

static __always_inline uint32_t
gx6605s_read(struct gx6605s_device *gdev, uint16_t reg)
{
    return readl(gdev->mmio + reg);
}

static __always_inline void
gx6605s_write(struct gx6605s_device *gdev, int reg, uint32_t val)
{
    writel(gdev->mmio + reg, val);
}

static state gx6605s_irq_pass(struct irqchip_device *idev, irqnr_t vector)
{
    struct gx6605s_device *gdev = irqchip_to_gdev(idev);

    if (vector < 32)
        gx6605s_write(gdev, GX6605S_INTC_GATESET0, BIT(vector));
    else
        gx6605s_write(gdev, GX6605S_INTC_GATESET1, BIT(vector - 32));

    return -ENOERR;
}

static state gx6605s_irq_mask(struct irqchip_device *idev, irqnr_t vector)
{
    struct gx6605s_device *gdev = irqchip_to_gdev(idev);

    if (vector < 8)
        gx6605s_write(gdev, GX6605S_INTC_GATECLR0, BIT(vector));
    else
        gx6605s_write(gdev, GX6605S_INTC_GATECLR1, BIT(vector - 32));

    return -ENOERR;
}

static struct irqchip_ops gx6605s_irq_ops = {
    .pass = gx6605s_irq_pass,
    .mask = gx6605s_irq_mask,
};

asmlinkage void generic_interrupt(struct regs *regs)
{
    unsigned long nr;

    while ((nr = fls(gx6605s_read(root, GX6605S_INTC_STAT1))))
        irq_handle(nr + 32);

    while ((nr = fls(gx6605s_read(root, GX6605S_INTC_STAT0))))
        irq_handle(nr);
}

static void gx6605s_setup_irqs(struct gx6605s_device *gdev, uint32_t magic)
{
    unsigned int count;
    for (count = 0; count < GX6605S_INTC_SOURCE_NR; count += 4)
        gx6605s_write(gdev, GX6605S_INTC_SOURCE0 + count, irq_source_val(count, magic));
}

static state gx6605s_probe(struct platform_device *pdev, void *pdata)
{
    resource_size_t start, size;

    if (root)
        return -EALREADY;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -EINVAL;

    root = dev_kzalloc(&pdev->dev, sizeof(*root), GFP_KERNEL);
    if (!root)
        return -ENOMEM;

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);

    root->mmio = dev_ioremap(&pdev->dev, start, size);
    if (!root->mmio)
        return -ENOMEM;

    /* disable all interrupt gate */
    gx6605s_write(root, GX6605S_INTC_GATE0, 0);
    gx6605s_write(root, GX6605S_INTC_GATE1, 0);

    /* disable all mask, because we only use gate */
    gx6605s_write(root, GX6605S_INTC_MASK0, 0);
    gx6605s_write(root, GX6605S_INTC_MASK1, 0);

    /* mask all fiq, because we don't handle fiq */
    gx6605s_write(root, GX6605S_FIQC_MASK0, 0xffffffff);
    gx6605s_write(root, GX6605S_FIQC_MASK1, 0xffffffff);

    gx6605s_setup_irqs(root, 0x03020100);

    root->irqchip.dev = &pdev->dev;
    root->irqchip.ops = &gx6605s_irq_ops;
    root->irqchip.dt_node = pdev->dt_node;
    return irqchip_register(&root->irqchip);
}

static struct dt_device_id gx6605s_intc_ids[] = {
    { .compatible = "nationalchip,gx6605s-intc" },
    { }, /* NULL */
};

static struct platform_driver gx6605s_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_intc_ids,
    .probe = gx6605s_probe,
};

static state gx6605s_init(void)
{
    return platform_driver_register(&gx6605s_driver);
}
irqchip_initcall(gx6605s_init);
