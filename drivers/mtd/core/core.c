/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/mtd.h>
#include <export.h>

SPIN_LOCK(mtd_lock);
LIST_HEAD(mtd_list);

state mtd_register(struct mtd_device *mdev, struct mtd_part *part, unsigned int pnr)
{
    unsigned int count;

    if (!mdev->dev || !mdev->ops)
        return -EINVAL;

    list_head_init(&mdev->parts);
    for (count = 0; count < pnr; ++count)
        list_add(&mdev->parts, &part[count].list);
    mtd_fsdev_register(mdev);

    spin_lock(&mtd_lock);
    list_add(&mtd_list, &mdev->list);
    spin_unlock(&mtd_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(mtd_register);

void mtd_unregister(struct mtd_device *mdev)
{

}
EXPORT_SYMBOL(mtd_unregister);
