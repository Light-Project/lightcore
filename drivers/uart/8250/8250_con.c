/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <init/initcall.h>
#include <console.h>
#include <driver/platform.h>
#include <driver/uart/8250.h>

#include <asm/io.h>
#include <asm/proc.h>

#define freq_def    115200

struct i8250_console {
    struct console console;
    unsigned int regshift;
    resource_size_t addr;
    bool mmio;
};

#define console_to_dev(con) \
    container_of(con, struct i8250_console, console)

static inline uint8_t i8250_in(struct i8250_console *dev, int reg)
{
    reg <<= dev->regshift;

    if (dev->mmio)
        return readb((void *)reg);
    return inb(reg);
}

static inline void i8250_out(struct i8250_console *dev, int reg, uint8_t val)
{
    reg <<= dev->regshift;

    if (dev->mmio)
        writeb((void *)reg, val);
    else
        outb(reg, val);
}

static void serial_putc(struct i8250_console *dev, char ch)
{
    while (!(i8250_in(dev, UART8250_LSR) & UART8250_LSR_THRE))
        cpu_relax();
    i8250_out(dev, UART8250_THR, ch);
}

void serial_putcs(struct console *con, const char *str, unsigned len)
{
    struct i8250_console *dev = console_to_dev(con);
    while (len--)
        serial_putc(dev, *str++);
}

static void i8250_init(struct i8250_console *dev)
{
    unsigned int clk = freq_def;

    i8250_out(dev, UART8250_IER, 0);
    i8250_out(dev, UART8250_MCR, UART8250_MCR_RTS | UART8250_MCR_DTR);
    i8250_out(dev, UART8250_FCR, UART8250_FCR_FIFO_EN);

    i8250_out(dev, UART8250_LCR, UART8250_LCR_DLAB);
    i8250_out(dev, UART8250_DLL, clk);
    i8250_out(dev, UART8250_DLH, clk >> 8);
    i8250_out(dev, UART8250_LCR, UART8250_LCR_WLS_8);
}

static state i8250_probe(struct platform_device *pdev)
{
    struct i8250_console *dev;
    int val;

    dev = kmalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    val = platform_resource_type(pdev, 0);
    if (val == RESOURCE_MMIO)
        dev->mmio = true;
    else if (val != RESOURCE_PMIO)
        return -ENODEV;

    dev->addr = platform_resource_start(pdev, 0);
    i8250_init(dev);

    dev->console.write = serial_putcs;
    console_register(&dev->console);
    return -ENOERR;
}

static struct dt_device_id i8250_ids[] = {
    { .compatible = "ns16550" },
    { .compatible = "ns16550a" },
    { .compatible = "snps,dw-apb-uart" },
    { }, /* NULL */
};

static struct platform_driver i8250_driver = {
    .driver = {
        .name = "i8250-console",
    },
    .dt_table = i8250_ids,
    .probe = i8250_probe,
};

static state i8250_con_init(void)
{
    return platform_driver_register(&i8250_driver);
}
console_initcall(i8250_con_init);
