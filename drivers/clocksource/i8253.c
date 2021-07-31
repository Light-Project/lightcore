/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i8253"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <kernel/irq.h>
#include <init/initcall.h>
#include <driver/platform.h>
#include <driver/clocksource.h>
#include <driver/clocksource/i8253.h>
#include <printk.h>

#include <asm/io.h>

#define I8253_BASE  0x40
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

static void pit_enable(void)
{
    pit_outb(I8253_MODE, 
        I8253_MODE_SEL_TIMER0 | I8253_MODE_ACCESS_WORD | 
        I8253_MODE_MOD_RATE   | I8253_MODE_CNT_BINARY  ); 
    pit_outb(I8253_COUNTER0, PIT_LATCH(CONFIG_SYSTICK_FREQ) & 0xff);
    pit_outb(I8253_COUNTER0, PIT_LATCH(CONFIG_SYSTICK_FREQ) >> 8);
}

static state i8253_probe(struct platform_device *pdev)
{
    irq_request(I8253_IRQ, 0, pit_handle, NULL, DRIVER_NAME);
    pit_enable();
    return -ENOERR;
}

static const struct dt_device_id i8253_device_id[] = {
    { .compatible = "i8253" },
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
