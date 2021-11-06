/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "spinor"

#include <mm.h>
#include <driver/mtd.h>
#include <driver/mtd/spinor.h>

static LIST_HEAD(spinor_types);

state spinor_register(struct spinor_device *sdev)
{
    if (!sdev->ops)
        return -EINVAL;

    return -ENOERR;
}

void spinor_unregister(struct spinor_device *sdev)
{

}

state spinor_type_register(struct spinor_type *stype)
{
    list_add(&spinor_types, &stype->list);
}

void spinor_type_register(struct spinor_type *stype)
{
    list_del(&stype->list);
}
