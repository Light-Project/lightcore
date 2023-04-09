/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <console.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/uart/8250.h>

#include <asm/io.h>
#include <asm/proc.h>

#define freq_def 115200

struct i8250_console {
    struct console console;
    unsigned int regshift;
    resource_size_t pmio;
    void *mmio;
};

#define console_to_dev(con) \
    container_of(con, struct i8250_console, console)

static inline uint8_t i8250con_in(struct i8250_console *dev, int reg)
{
    reg <<= dev->regshift;

    if (dev->mmio)
        return readb(dev->mmio + reg);
    else
        return inb(dev->pmio + reg);
}

static inline void i8250con_out(struct i8250_console *dev, int reg, uint8_t val)
{
    reg <<= dev->regshift;

    if (dev->mmio)
        writeb(dev->mmio + reg, val);
    else
        outb(dev->pmio + reg, val);
}

static void i8250con_putc(struct i8250_console *dev, char ch)
{
    while (!(i8250con_in(dev, UART8250_LSR) & UART8250_LSR_THRE))
        cpu_relax();
    i8250con_out(dev, UART8250_THR, ch);
}

static void i8250con_putcs(struct console *con, const char *str, unsigned len)
{
    struct i8250_console *dev = console_to_dev(con);
    while (len--)
        i8250con_putc(dev, *str++);
}

static state i8250con_setup(struct console *con)
{
    struct i8250_console *dev = console_to_dev(con);

    if (0) {
        unsigned int clk = freq_def;
        i8250con_out(dev, UART8250_LCR, UART8250_LCR_DLAB);
        i8250con_out(dev, UART8250_DLL, clk);
        i8250con_out(dev, UART8250_DLH, clk >> 8);
    }

    i8250con_out(dev, UART8250_IER, 0);
    i8250con_out(dev, UART8250_MCR, UART8250_MCR_RTS | UART8250_MCR_DTR);
    i8250con_out(dev, UART8250_FCR, UART8250_FCR_FIFO_EN);
    i8250con_out(dev, UART8250_LCR, UART8250_LCR_WLS_8);

    return -ENOERR;
}

static struct console_ops i8250con_ops = {
    .write = i8250con_putcs,
    .startup = i8250con_setup,
};

static state i8250con_probe(struct platform_device *pdev, const void *pdata)
{
    struct i8250_console *dev;
    resource_size_t addr, size;
    int val;

    val = platform_resource_type(pdev, 0);
    if (val != RESOURCE_MMIO)
        return -ENODEV;

    dev = dev_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    addr = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    dev->mmio = dev_ioremap(&pdev->dev, addr, size);
    if (!dev->mmio)
        return -ENOMEM;

    dev->regshift = 4;
    dev->console.ops = &i8250con_ops;
    dev->console.flags = CONSOLE_BOOT;
    console_register(&dev->console);
    return -ENOERR;
}

static struct dt_device_id i8250con_ids[] = {
    { .compatible = "ns16550" },
    { .compatible = "ns16550a" },
    { .compatible = "snps,dw-apb-uart" },
    { }, /* NULL */
};

static struct platform_driver i8250con_driver = {
    .driver = {
        .name = "i8250-console",
    },
    .dt_table = i8250con_ids,
    .probe = i8250con_probe,
};

static state i8250con_init(void)
{
    return platform_driver_register(&i8250con_driver);
}
console_initcall(i8250con_init);
