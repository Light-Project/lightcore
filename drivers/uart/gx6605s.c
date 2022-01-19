/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-uart"

#include <kmalloc.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/uart.h>
#include <driver/uart/gx6605s.h>
#include <asm/io.h>

static struct dt_device_id gx6605s_uart_ids[] = {
    { .compatible = "nationalchip,gx6605s-uart" },
    { }, /* NULL */
};

static struct platform_driver gx6605s_uart_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_uart_ids,
    .probe  = gx6605s_uart_probe,
    .remove = gx6605s_uart_remove,
};

static state gx6605s_uart_init(void)
{
    return platform_driver_register(&gx6605s_uart_driver);
}
driver_initcall(gx6605s_uart_init);
