/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "spinor"

#include <memory.h>
#include <driver/mtd.h>
#include <driver/mtd/spinor.h>

static LIST_HEAD(spinor_types);

void spinor_type_register(struct spinor_type *stype)
{
    while (stype->jedec && stype->name) {
        list_add(&spinor_types, &stype->list);
        ++stype;
    }
}

void spinor_type_unregister(struct spinor_type *stype)
{
    while (stype->jedec && stype->name) {
        list_del(&stype->list);
        ++stype;
    }
}

state spinor_register(struct spinor_device *sdev)
{
    if (!sdev->ops)
        return -EINVAL;

    return -ENOERR;
}

void spinor_unregister(struct spinor_device *sdev)
{

}
