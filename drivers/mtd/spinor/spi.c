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

struct spinor_transfer {
    struct {
        uint8_t len;
        uint16_t opcode;
    } cmd;

    struct {
        uint8_t len;
        uint64_t addr;
    } addr;

    struct {
        uint8_t len;

    } dummy;

    struct {
        uint8_t len;
        void *buffer;
    } data;
};

static state spinor_spi_transfer(struct spi_device *spi, struct spinor_transfer *tran)
{

}

static state spinor_spi_regread(struct spinor_device *nor, loff_t pos, uint8_t *buf, uint8_t len)
{
    return -ENOERR;
}

static state spinor_spi_regwrite(struct spinor_device *nor, loff_t pos, uint8_t *buf, uint8_t len)
{
    return -ENOERR;
}

static state spinor_spi_read(struct spinor_device *nor, loff_t pos, uint8_t *buf, uint64_t len)
{
    return -ENOERR;
}

static state spinor_spi_write(struct spinor_device *nor, loff_t pos, uint8_t *buf, uint64_t len)
{
    return -ENOERR;
}

static state spinor_spi_erase(struct spinor_device *nor, loff_t pos, uint64_t len)
{
    struct spinor_transfer transfer = {
        .cmd = {
            .opcode = nor->erase_op,
        },
        .addr = {

        }

    };

    return -ENOERR;
}

static struct spinor_ops spinor_ops = {
    .reg_read = spinor_spi_regread,
    .reg_write = spinor_spi_regwrite,
    .read = spinor_spi_read,
    .write = spinor_spi_write,
    .erase = spinor_spi_erase,
};

static state spinor_spi_probe(struct *)
{
    struct spinor_device *sdev;

    kmall

    return spinor_register();
}

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

static state spinor_spi_initcall(void)
{
    spi_driver_register(&spinor_driver);
}
driver_initcall(spinor_initcall);
