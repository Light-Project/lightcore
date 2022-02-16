/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i8259"

#include <initcall.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/irqchip/i8259.h>
#include <delay.h>
#include <asm/io.h>

struct i8259_device {
    struct irqchip_device irqchip;
    struct irqchip_channel *parent;
    resource_size_t base;
    uint8_t mask, slave;
    spinlock_t lock;
};

#define irqchip_to_idev(irq) \
    container_of(irq, struct i8259_device, irqchip)

#define I8259_IRQ_NR 8

static __always_inline uint8_t
i8259_in(struct i8259_device *i8259, int reg)
{
    uint8_t val = inb(i8259->base + reg);

    /*
     * delay for some accesses to PIC on motherboard or
     * in chipset must be at least one microsecond.
     */
    udelay(2);

    return val;
}

static __always_inline void
i8259_out(struct i8259_device *i8259, int reg, uint8_t val)
{
    outb(i8259->base + reg, val);

    /*
     * delay for some accesses to PIC on motherboard or
     * in chipset must be at least one microsecond.
     */
    udelay(2);
}

static void i8259_mask(struct i8259_device *i8259, uint8_t off, uint8_t on)
{
    i8259->mask = (i8259->mask & ~off) | on;
    i8259_out(i8259, I8259_DATA, i8259->mask);
}

static state i8259_irq_pass(struct irqchip_device *idev, irqnr_t vector)
{
    struct i8259_device *i8259 = irqchip_to_idev(idev);

    if (vector > I8259_IRQ_NR - 1)
        return -EINVAL;

    spin_lock(&i8259->lock);
    i8259_mask(i8259, BIT(vector), 0);
    spin_unlock(&i8259->lock);

    return -ENOERR;
}

static state i8259_irq_mask(struct irqchip_device *idev, irqnr_t vector)
{
    struct i8259_device *i8259 = irqchip_to_idev(idev);

    if (vector > I8259_IRQ_NR - 1)
        return -EINVAL;

    spin_lock(&i8259->lock);
    i8259_mask(i8259, 0, BIT(vector));
    spin_unlock(&i8259->lock);

    return -ENOERR;
}

static state i8259_irq_eoi(struct irqchip_device *idev, irqnr_t vector)
{
    struct i8259_device *i8259 = irqchip_to_idev(idev);

    if (vector > I8259_IRQ_NR - 1)
        return -EINVAL;

    spin_lock(&i8259->lock);

    /* starts the control sequence */
    i8259_mask(i8259, 0, 0);
    i8259_out(i8259, I8259_CMD, I8259_OCW2_SL |
        I8259_OCW2_EOI | (I8259_OCW2_VEC & vector));

    spin_unlock(&i8259->lock);
    return -ENOERR;
}

static state i8259_slave_add(struct irqchip_device *idev, irqnr_t vector)
{
    struct i8259_device *i8259 = irqchip_to_idev(idev);

    if (vector > I8259_IRQ_NR - 1)
        return -EINVAL;

    spin_lock(&i8259->lock);

    i8259->slave |= BIT(vector);

    /* starts the initialization sequence */
    i8259_out(i8259, I8259_CMD,  I8259_ICW1_INIT | I8259_ICW1_ICW4);
    i8259_out(i8259, I8259_DATA, IRQ_EXTERNAL);
    i8259_out(i8259, I8259_DATA, i8259->slave);
    i8259_out(i8259, I8259_DATA, I8259_ICW4_AUTO | I8259_ICW4_8086);

    spin_unlock(&i8259->lock);
    i8259_irq_pass(idev, vector);
    return -ENOERR;
}

static state i8259_slave_del(struct irqchip_device *idev, irqnr_t vector)
{
    struct i8259_device *i8259 = irqchip_to_idev(idev);

    if (vector > I8259_IRQ_NR - 1)
        return -EINVAL;

    spin_lock(&i8259->lock);

    i8259->slave &= ~BIT(vector);

    /* starts the initialization sequence */
    i8259_out(i8259, I8259_CMD,  I8259_ICW1_INIT | I8259_ICW1_ICW4);
    i8259_out(i8259, I8259_DATA, IRQ_EXTERNAL);
    i8259_out(i8259, I8259_DATA, i8259->slave);
    i8259_out(i8259, I8259_DATA, I8259_ICW4_AUTO | I8259_ICW4_8086);

    spin_unlock(&i8259->lock);
    i8259_irq_mask(idev, vector);
    return -ENOERR;
}

static struct irqchip_ops pic_ops = {
    .pass = i8259_irq_pass,
    .mask = i8259_irq_mask,
    .eoi = i8259_irq_eoi,
    .slave_add = i8259_slave_add,
    .slave_del = i8259_slave_del,
};

static void i8250_hw_init(struct platform_device *pdev)
{
    struct i8259_device *i8259 = platform_get_devdata(pdev);
    uint32_t index;

    /* mask all interrupt */
    i8259_mask(i8259, 0, 0xff);

    /* starts the initialization sequence */
    if (!dt_attribute_read_u32(pdev->dt_node, "interrupts", &index)) {
        i8259_out(i8259, I8259_CMD,  I8259_ICW1_INIT | I8259_ICW1_ICW4);
        i8259_out(i8259, I8259_DATA, IRQ_EXTERNAL + 8);
        i8259_out(i8259, I8259_DATA, index & I8259_ICW3_SLVD);
        i8259_out(i8259, I8259_DATA, I8259_ICW4_AUTO | I8259_ICW4_8086);
    } else { /* master mode */
        i8259_out(i8259, I8259_CMD,  I8259_ICW1_INIT | I8259_ICW1_ICW4);
        i8259_out(i8259, I8259_DATA, IRQ_EXTERNAL);
        /* all interrupts should be prohibited, but bochs will panic */
        i8259_out(i8259, I8259_DATA, BIT(2) & I8259_ICW3_SLVD);
        i8259_out(i8259, I8259_DATA, I8259_ICW4_AUTO | I8259_ICW4_8086);
    }

    /* mask all interrupt again */
    i8259_mask(i8259, 0, 0xff);
}

static state i8259_probe(struct platform_device *pdev, const void *pdata)
{
    struct i8259_device *idev;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -EINVAL;

    idev = dev_kzalloc(&pdev->dev, sizeof(*idev), GFP_KERNEL);
    if(!idev)
        return -ENOMEM;

    idev->base = platform_resource_start(pdev, 0);
    idev->irqchip.ops = &pic_ops;
    idev->irqchip.dev = &pdev->dev;
    idev->irqchip.dt_node = pdev->dt_node;
    platform_set_devdata(pdev, idev);

    i8250_hw_init(pdev);
    return irqchip_register(&idev->irqchip);
}

static struct dt_device_id i8259_id[] = {
    { .compatible = "intel,i8259" },
    { }, /* NULL */
};

static struct platform_driver i8259_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i8259_id,
    .probe = i8259_probe,
};

static state i8259_init(void)
{
    return platform_driver_register(&i8259_driver);
}
irqchip_initcall(i8259_init);
