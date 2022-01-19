/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "iodelay"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <asm/io.h>
#include <asm/delay.h>
#include <driver/firmware/dmi.h>
#include <export.h>
#include <printk.h>

enum io_delay_type {
    IO_DELAY_TYPE_0X80      = 0,
    IO_DELAY_TYPE_0XED      = 1,
    IO_DELAY_TYPE_UDELAY    = 2,
    IO_DELAY_TYPE_NONE      = 3,
};

static const char *io_delay_name[] = {
    "" "0xed", "udelay", "none",
};

#if defined(CONFIG_IO_DELAY_0X80)
# define DEFAULT_IO_DELAY_TYPE IO_DELAY_TYPE_0X80
#elif defined(CONFIG_IO_DELAY_0XED)
# define DEFAULT_IO_DELAY_TYPE IO_DELAY_TYPE_0XED
#elif defined(CONFIG_IO_DELAY_UDELAY)
# define DEFAULT_IO_DELAY_TYPE IO_DELAY_TYPE_UDELAY
#elif defined(CONFIG_IO_DELAY_NONE)
# define DEFAULT_IO_DELAY_TYPE IO_DELAY_TYPE_NONE
#endif

static int io_delay_type = DEFAULT_IO_DELAY_TYPE;

void io_delay(void)
{
    switch (io_delay_type) {
        case IO_DELAY_TYPE_0X80: default:
            asm volatile("outb %al, $0x80");
            break;
        case IO_DELAY_TYPE_0XED:
            asm volatile("outb %al, $0xed");
            break;
        case IO_DELAY_TYPE_UDELAY:
            udelay(2);
            break;
        case IO_DELAY_TYPE_NONE:
            break;
    }
}
EXPORT_SYMBOL(io_delay);

static struct dmi_device_id io_delay_spec_ids[] = {
    {
        .name = "Compaq Presario V6000",
        .matches = {
            DMI_MATCH(DMI_REG_BOARD_VENDOR, "Quanta"),
            DMI_MATCH(DMI_REG_BOARD_NAME, "30B7"),
        },
        .data = (void *)IO_DELAY_TYPE_0XED,
    }, {
        .name = "HP Pavilion dv9000z",
        .matches = {
            DMI_MATCH(DMI_REG_BOARD_VENDOR, "Quanta"),
            DMI_MATCH(DMI_REG_BOARD_NAME, "30B9"),
        },
        .data = (void *)IO_DELAY_TYPE_0XED,
    }, {
        .name = "HP Pavilion dv6000",
        .matches = {
            DMI_MATCH(DMI_REG_BOARD_VENDOR, "Quanta"),
            DMI_MATCH(DMI_REG_BOARD_NAME, "30B8"),
        },
        .data = (void *)IO_DELAY_TYPE_0XED,
    }, {
        .name = "HP Pavilion tx1000",
        .matches = {
            DMI_MATCH(DMI_REG_BOARD_VENDOR, "Quanta"),
            DMI_MATCH(DMI_REG_BOARD_NAME, "30BF"),
        },
        .data = (void *)IO_DELAY_TYPE_0XED,
    }, {
        .name = "Presario F700",
        .matches = {
            DMI_MATCH(DMI_REG_BOARD_VENDOR, "Quanta"),
            DMI_MATCH(DMI_REG_BOARD_NAME, "30D3"),
        },
        .data = (void *)IO_DELAY_TYPE_0XED,
    },
    { }, /* NULL */
};

void __init io_delay_init(void)
{
    enum io_delay_type type;
    const char *old, *new;

    type = (size_t)dmi_system_check(io_delay_spec_ids);
    if (!type)
        return;

    old = io_delay_name[io_delay_type];
    new = io_delay_name[type];
    pr_notice("fixup type (%s -> %s)\n", old, new);

    io_delay_type = type;
}
