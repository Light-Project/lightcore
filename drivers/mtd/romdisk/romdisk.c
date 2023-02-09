/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "romdisk"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <memory.h>
#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/mtd.h>

static state romdisk_read(struct mtd_device *mtd, loff_t pos, void *buf, uint64_t len, uint64_t *retlen)
{
    memcpy(buf, _ld_romdisk_start + pos, len);
    *retlen = len;
    return -ENOERR;
}

static struct mtd_ops romdisk_ops = {
    .read = romdisk_read,
};

static struct mtd_part romdisk_part = {
    .name = "romdisk",
};

static state romdisk_probe(struct platform_device *pdev, const void *pdata)
{
    struct mtd_device *mdev;

    mdev = kzalloc(sizeof(*mdev), GFP_KERNEL);
    if (!mdev)
        return -ENOMEM;

    mdev->dev = &pdev->dev;
    mdev->ops = &romdisk_ops;
    mdev->size = ROMDISK_SIZE;
    romdisk_part.size = ROMDISK_SIZE;

    return mtd_register(mdev, &romdisk_part, 1);
}

static struct platform_driver romdisk_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = romdisk_probe,
};

static state romdisk_init(void)
{
    struct platform_device *pdev;

    pdev = platform_unified_register(&romdisk_driver, NULL, 0);
    if (!pdev)
        return -ENOMEM;

    return -ENOERR;
}
driver_initcall(romdisk_init);
