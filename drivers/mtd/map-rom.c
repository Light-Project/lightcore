/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "mtd-rom"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/mtd.h>

struct rom_device {

};

static state rom_mtd_read(struct mtd_device *mtd, loff_t pos, void *buf, uint64_t len)
{
    return -ENOERR;
}

static struct mtd_ops rom_mtd_ops = {
    .read = rom_mtd_read,
};

static state rom_mtd_probe(struct platform_device *pdev, const void *pdata)
{
    return -ENOERR;
}

static struct dt_device_id rom_mtd_ids[] = {
    { .compatible = "mtd-rom" },
    { }, /* NULL */
};

static struct platform_driver rom_mtd_driver = {
    .driver = {
        .name = "mtd-rom",
    },
    .probe = rom_mtd_probe,
    .dt_table = rom_mtd_ids,
};

static state map_rom_init(void)
{
    return platform_driver_register(&rom_mtd_driver);
}
driver_initcall(map_rom_init);
