/* SPDX-License-Identifier: GPL-2.0-or-later */

#define DRIVER_NAME "i8253"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <system/irq.h>
#include <init/initcall.h>
#include <driver/platform.h>
#include <driver/clocksource.h>
#include <driver/clocksource/i8253.h>

#include <asm/io.h>

static __always_inline uint8_t pit_inb(int reg)
{
    return inb(I8253_BASE + reg);
}

static __always_inline void pit_outb(int reg, uint8_t value)
{
    outb(I8253_BASE + reg, value);
}

static irq_return_t pit_handle(irqnr_t vector, void *data)
{
    return IRQ_RET_HANDLED;
}

static state pit_enable(struct clocksource_device *cdev)
{
    pit_outb(I8253_MODE, 
        I8253_MODE_SEL_TIMER0 | I8253_MODE_ACCESS_WORD | 
        I8253_MODE_MOD_RATE   | I8253_MODE_CNT_BINARY  ); 
    pit_outb(I8253_COUNTER0, 0x00);
    pit_outb(I8253_COUNTER0, 0x00);
    return -ENOERR;
}

static state pit_disable(struct clocksource_device *cdev)
{
    pit_outb(I8253_MODE, I8253_MODE_SEL_TIMER0);
    return -ENOERR;
}

static struct clocksource_ops pit_ops = {
    .enable = pit_enable,
    .disable = pit_disable,
};

static state i8253_probe(struct platform_device *pdev)
{
    struct clocksource_device *cdev;

    cdev = kmalloc(sizeof(*cdev), GFP_KERNEL);
    if(!cdev)
        return -ENOMEM;

    irq_request(I8253_IRQ, 0, pit_handle, NULL, DRIVER_NAME);
    cdev->ops = &pit_ops;
    clocksource_register(cdev);   
    
    return -ENOERR;
}

static const struct dt_device_id i8253_device_id[] = {
    {.compatible = "i8253"},
    { }, /* NULL */
};

static struct platform_driver i8253_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i8253_device_id,
    .probe = i8253_probe,
};

state i8253_init(void)
{
    return platform_driver_register(&i8253_driver);
}
// clocksource_initcall(i8254_init);

