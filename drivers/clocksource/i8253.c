/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i8253"

#include <kmalloc.h>
#include <initcall.h>
#include <irq.h>
#include <timer.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/clocksource.h>
#include <driver/clocksource/i8253.h>
#include <printk.h>

#include <asm/io.h>

struct i8253_device {
    struct irqchip_channel *irqchip;
    resource_size_t base;
};

static __always_inline uint8_t
i8253_in(struct i8253_device *idev, int reg)
{
    return inb(idev->base + reg);
}

static __always_inline void
i8253_out(struct i8253_device *idev, int reg, uint8_t value)
{
    outb(idev->base + reg, value);
}

static irqreturn_t pit_handle(irqnr_t vector, void *data)
{
    timer_tick();
    return IRQ_RET_HANDLED;
}

static state i8253_probe(struct platform_device *pdev, void *pdata)
{
    struct i8253_device *idev;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENODEV;

    idev = dev_kzalloc(&pdev->dev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;
    platform_set_devdata(pdev, idev);

    idev->base = platform_resource_start(pdev, 0);
    idev->irqchip = dt_get_irqchip_channel(pdev->dt_node, 0);
    if (!idev->irqchip) {
        dev_err(&pdev->dev, "unable to request irq\n");
        return -ENODEV;
    }
    irqchip_pass(idev->irqchip);
    irq_request(I8253_IRQ, 0, pit_handle, idev, DRIVER_NAME);

    i8253_out(idev, I8253_MODE, I8253_MODE_SEL_TIMER0 | I8253_MODE_ACCESS_WORD |
                                I8253_MODE_MOD_RATE   | I8253_MODE_CNT_BINARY  );
    i8253_out(idev, I8253_COUNTER0, I8253_LATCH(CONFIG_SYSTICK_FREQ) & 0xff);
    i8253_out(idev, I8253_COUNTER0, I8253_LATCH(CONFIG_SYSTICK_FREQ) >> 8);

    return -ENOERR;
}

static const struct dt_device_id i8253_device_id[] = {
    { .compatible = "intel,i8253" },
    { }, /* NULL */
};

static struct platform_driver i8253_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i8253_device_id,
    .probe = i8253_probe,
};

static state i8253_init(void)
{
    return platform_driver_register(&i8253_driver);
}
clocksource_initcall(i8253_init);
