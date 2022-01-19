/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <driver/mtd.h>

#define romdisk_size (&_ld_romdisk_end - &_ld_romdisk_start)
static struct mtd_part romdisk_part;

static state romdisk_read(struct mtd_device *mtd, loff_t pos, void *buf, uint64_t len, uint64_t *retlen)
{
    memcpy(buf, &_ld_romdisk_start + pos, len);
    *retlen = len;
    return -ENOERR;
}

static struct mtd_ops romdisk_ops = {
    .read = romdisk_read,
};

static state romdisk_init(void)
{
    struct mtd_device *mdev;

    mdev = kzalloc(sizeof(*mdev), GFP_KERNEL);
    if (!mdev)
        return -ENOMEM;

    romdisk_part.size = romdisk_size;
    mdev->size = romdisk_size;
    mdev->ops = &romdisk_ops;
    return mtd_register(mdev);
}
driver_initcall(romdisk_init);
