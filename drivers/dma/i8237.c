/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i8237"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <init/initcall.h>
#include <driver/platform.h>
#include <driver/dma.h>
#include <printk.h>

#include <asm/io.h>


static struct dma_ops i8237_ops {

};

static state i8237_prebe(struct platform_device *pdev)
{

    return -ENOERR;
}

static state i8237_remove(struct platform_device *pdev)
{

    return -ENOERR;
}

static struct dt_device_id i8237_id_table[] = {
    { .compatible = "i8237" },
    { }, /* NULL */
};

static struct platform_driver i8237_driver {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i8237_id_table,
    .probe = i8237_prebe,
    .remove = i8237_remove,
};

static state i8237_init(void)
{
    return platform_driver_register(&i8237_driver);
}
arch_initcall(i8237_init);
