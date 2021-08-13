/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "generic-gpio"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <init/initcall.h>
#include <driver/platform.h>
#include <driver/gpio.h>

static state ggpio_probe(struct platform_device *pdev)
{
    
    return -ENOERR;
}

static struct dt_device_id ggpio_ids[] = {
    { .compatible = "wd,mbl-gpio" },
    { }, /* NULL */
};

static struct platform_driver ggpio_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = ggpio_ids,
    .probe = ggpio_probe,
};

static state ggpio_init(void)
{
    return platform_driver_register(&ggpio_driver);
}
driver_initcall(ggpio_init);
