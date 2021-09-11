/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ata-simple"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <initcall.h>
#include <driver/platform.h>
#include <printk.h>

static state i8042_probe(struct platform_device *pdev)
{

    return -ENOERR;
}

static struct platform_driver i8042_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = i8042_probe,
};

static state i8042_init(void)
{
    return platform_driver_register(&i8042_driver);
}
driver_initcall(i8042_init);
