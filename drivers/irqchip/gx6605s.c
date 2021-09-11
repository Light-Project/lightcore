/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-intc"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <string.h>
#include <mm.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/irqchip/gx6605s.h>
#include <printk.h>

#include <asm/io.h>
#include <asm/irq.h>

struct gx6605s_device {
    struct irqchip_device irq;
    void *mmio;
};

#define irq_to_gdev(irq_device) \
    container_of(irq_device, struct gx6605s_device, irq)

static inline uint32_t
gx6605s_read(struct gx6605s_device *gdev, uint16_t reg)
{
    return readl(gdev->mmio + reg);
}

static void gx6605s_write(struct gx6605s_device *gdev, int reg, uint32_t val)
{
    writel(gdev->mmio + reg, val);
}


#define irq_channel_val(irq, magic) (irq | (irq << 8) | (irq << 16) | (irq << 24) | magic)
static void gx6605s_setup_irqs(struct gx6605s_device *gdev, uint32_t magic)
{
    for (int count = 0; count < GX6605S_IRQS; count += 4)
        gx6605s_write(gdev, GX6605S_INTC_SOURCE + count, irq_channel_val(count, magic));
}

static state gx6605s_probe(struct platform_device *pdev)
{
    struct gx6605s_device *gdev;
    resource_size_t start, size;
    int val;

    val = platform_resource_type(pdev, 0);
    if (val != RESOURCE_MMIO)
        return -ENODEV;

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);

    gdev = kzalloc(sizeof(*gdev), GFP_KERNEL);
    if (!gdev)
        return -ENOMEM;
    platform_set_devdata(pdev, gdev);

    gdev->mmio = ioremap(start, size);
    if (!gdev->mmio) {
        kfree(gdev);
        return -ENOMEM;
    }

    /* Initial enable reg to disable all interrupts */
    gx6605s_write(gdev, GX6605S_INTC_NEN31_00, 0);
    gx6605s_write(gdev, GX6605S_INTC_NEN63_32, 0);

    /* Initial mask reg with all unmasked, because we only use enalbe reg */
    gx6605s_write(gdev, GX6605S_INTC_NMASK31_00, 0);
    gx6605s_write(gdev, GX6605S_INTC_NMASK63_32, 0);

    gx6605s_setup_irqs(gdev, 0x03020100);

    return -ENOERR;
}

static void gx6605s_remove(struct platform_device *pdev)
{
    struct gx6605s_device *gdev = platform_get_devdata(pdev);

    gx6605s_write(gdev, GX6605S_INTC_NEN31_00, 0);
    gx6605s_write(gdev, GX6605S_INTC_NEN63_32, 0);

    iounmap(gdev->mmio);
    kfree(gdev);
}

static struct dt_device_id gx6605s_id[] = {
    { .compatible = "csky,gx6605s-intc" },
    { }, /* NULL */
};

static struct platform_driver gx6605s_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_id,
    .probe = gx6605s_probe,
    .remove = gx6605s_remove,
};

static state gx6605s_init(void)
{
    return platform_driver_register(&gx6605s_driver);
}
irqchip_initcall(gx6605s_init);
