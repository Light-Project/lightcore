/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605-timer"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <kernel/irq.h>
#include <init/initcall.h>
#include <driver/platform.h>
#include <driver/clocksource.h>
#include <driver/clocksource/gx6605s.h>
#include <printk.h>

#include <asm/io.h>

static const struct dt_device_id gx6605_device_id[] = {
    { .compatible = "csky,gx6605s-timer" },
    { }, /* NULL */
};

static struct platform_driver gx6605_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605_device_id,
//     .probe = gx6605_probe,
};

static state gx6605_init(void)
{
    return platform_driver_register(&gx6605_driver);
}
clocksource_initcall(gx6605_init);
