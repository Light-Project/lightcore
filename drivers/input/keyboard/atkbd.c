/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "atkeyboard"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/serio.h>
#include <driver/input/atkbd.h>

static const unsigned short atkbd_keymap[] = {
      0, 67, 65, 63, 61, 59, 60, 88,  0, 68, 66, 64, 62, 15, 41,117,
      0, 56, 42, 93, 29, 16,  2,  0,  0,  0, 44, 31, 30, 17,  3,  0,
      0, 46, 45, 32, 18,  5,  4, 95,  0, 57, 47, 33, 20, 19,  6,183,
      0, 49, 48, 35, 34, 21,  7,184,  0,  0, 50, 36, 22,  8,  9,185,
      0, 51, 37, 23, 24, 11, 10,  0,  0, 52, 53, 38, 39, 25, 12,  0,
      0, 89, 40,  0, 26, 13,  0,  0, 58, 54, 28, 27,  0, 43,  0, 85,
      0, 86, 91, 90, 92,  0, 14, 94,  0, 79,124, 75, 71,121,  0,  0,
     82, 83, 80, 76, 77, 72,  1, 69, 87, 78, 81, 74, 55, 73, 70, 99,
};

static irqreturn_t atkbd_interrupt(struct serio_device *sdev, uint32_t data, const void *pdata)
{
    printk("%d\n", atkbd_keymap[data]);
    return IRQ_RET_HANDLED;
}

static state atkbd_probe(struct serio_device *sdev, const void *pdata)
{
    return -ENOERR;
}

static const struct serio_device_id atkbd_ids[] = {
    { SERIO_DEVICE(SERIO_TYPE_PS2) },
    { SERIO_DEVICE(SERIO_TYPE_PS2_XL) },
    { }, /* NULL */
};

static struct serio_driver atkbd_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = atkbd_ids,
    .probe = atkbd_probe,
    .interrupt = atkbd_interrupt,
};

static state atkbd_init(void)
{
    return serio_driver_register(&atkbd_driver);
}
driver_initcall(atkbd_init);
