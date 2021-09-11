/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "uart8250-isa"

#include <mm.h>
#include <initcall.h>
#include <driver/platform.h>
#include "8250.h"

#include <asm/io.h>

struct uart8250_isa_device {
    struct uart8250_device uart8250;
    resource_size_t port;
};

#define uart8250_to_isa(uart8250) \
    container_of(uart8250, struct uart8250_isa_device, uart8250);

static uint32_t uart8250_isa_in(struct uart8250_device *uart8250, uint8_t port)
{
    struct uart8250_isa_device *isa = uart8250_to_isa(uart8250);
    return inb(isa->port + port);
}

static void uart8250_isa_out(struct uart8250_device *uart8250, uint8_t port, uint32_t val)
{
    struct uart8250_isa_device *isa = uart8250_to_isa(uart8250);
    outb(isa->port + port, val);
}

struct uart8250_ops uart8250_isa_ops = {
    .port_in = uart8250_isa_in,
    .port_out = uart8250_isa_out,
};

static state uart8250_isa_probe(struct platform_device *pdev)
{
    struct uart8250_isa_device *isa;

    isa = dev_kzalloc(&pdev->device, sizeof(*isa), GFP_KERNEL);
    if (!isa)
        return -ENOMEM;
    platform_set_devdata(pdev, isa);

    isa->uart8250.ops = &uart8250_isa_ops;
    return uart8250_register(&isa->uart8250);
}

static void uart8250_isa_remove(struct platform_device *pdev)
{
    struct uart8250_isa_device *isa = platform_get_devdata(pdev);
    uart8250_unregister(&isa->uart8250);
}

static struct dt_device_id uart8250_isa_ids[] = {
    { .compatible = "intel,i8250" },
    { }, /* NULL */
};

static struct platform_driver uart8250_isa_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = uart8250_isa_ids,
    .probe = uart8250_isa_probe,
    .remove = uart8250_isa_remove,
};

static state uart8250_isa_init(void)
{
    return platform_driver_register(&uart8250_isa_driver);
}
driver_initcall(uart8250_isa_init);
