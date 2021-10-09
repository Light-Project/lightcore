/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "atkeyboard"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/serio.h>
#include <driver/input/atkbd.h>

static irqreturn_t atkbd_interrupt(struct serio_device *sdev, uint32_t data, void *pdata)
{

    return IRQ_RET_HANDLED;
}

static state atkbd_probe(struct serio_device *sdev, void *pdata)
{

    return -ENOERR;
}

static struct serio_driver atkbd_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = atkbd_probe,
    .interrupt = atkbd_interrupt,
};

static state atkbd_init(void)
{
    return serio_driver_register(&atkbd_driver);
}
driver_initcall(atkbd_init);
