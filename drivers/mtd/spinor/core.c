/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "spinor"

#include <mm.h>
#include <initcall.h>
#include <driver/spi.h>
#include <driver/mtd.h>
#include <driver/mtd/spinor.h>

static state spinor_read(struct mtd_info *mtd, loff_t pos, void *buf, size_t len)
{

}

static state spinor_write(struct mtd_info *mtd, loff_t pos, void *buf, size_t len)
{

}

static struct mtd_ops spinor_ops = {
    .read = spinor_read,
    .write = spinor_write,
};

static struct dt_device_id spinor_ids[] = {
    { .compatible = "jedec,spi-nor" },
    { }, /* NULL */
};

static struct spi_driver spinor_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = spinor_ids,
};

static state spinor_initcall(void)
{
    spi_driver_register(&spinor_driver);
}
driver_initcall(spinor_initcall);
