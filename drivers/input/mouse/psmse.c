/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "psmse"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/serio.h>
#include <driver/input/psmse.h>

static irqreturn_t psmse_interrupt(struct serio_device *sdev, uint32_t data, const void *pdata)
{
    printk("%d\n", data);
    return IRQ_RET_HANDLED;
}

static state psmse_probe(struct serio_device *sdev, const void *pdata)
{

    return -ENOERR;
}

static const struct serio_device_id psmse_ids[] = {
    { SERIO_DEVICE(SERIO_TYPE_PS2) },
    { SERIO_DEVICE(SERIO_TYPE_PS2_XL) },
    { }, /* NULL */
};

static struct serio_driver psmse_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = psmse_ids,
    .probe = psmse_probe,
    .interrupt = psmse_interrupt,
};

static state psmse_init(void)
{
    return serio_driver_register(&psmse_driver);
}
driver_initcall(psmse_init);
