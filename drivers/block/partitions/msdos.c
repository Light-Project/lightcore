/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "msdos-part: " fmt

#include <kmalloc.h>
#include <initcall.h>
#include <device.h>
#include <driver/block.h>
#include <driver/block/partition.h>
#include <driver/block/msdos.h>
#include <printk.h>

#include <lightcore/asm/byteorder.h>

static inline bool msdos_is_extended(uint8_t type)
{
    return (type == MSDOS_DOS_EXT_PART   ||
            type == MSDOS_WIN98_EXT_PART ||
            type == MSDOS_LINUX_EXT_PART);
}

static state msdos_scan_extended(struct block_device *bdev, uint32_t pos)
{
    struct block_part *entry;
    struct msdos_head *msdos;
    int count, part;
    state ret;

    msdos = kmalloc(sizeof(*msdos), GFP_KERNEL);
    if (!msdos)
        return -ENOMEM;

    for (count = 0; count < 100; count++) {
        if ((ret = block_device_read(bdev, msdos, pos + count, 1)))
            break;

        if (MSDOS_MAGIC != cpu_to_be16(msdos->magic))
            break;

        for (part = 0; part < 4; ++part) {
            uint32_t start, size;

            start = msdos->dpt[part].lba;
            size = msdos->dpt[part].size;

            if (!start || !size ||
                msdos_is_extended(msdos->dpt[part].type))
                continue;

            entry = kzalloc(sizeof(*entry), GFP_KERNEL);
            if (!entry) {
                ret = -ENOMEM;
                break;
            }

            entry->start = start + pos;
            entry->len = size;
            list_add_prev(&bdev->parts, &entry->list);
            dev_info(bdev->dev, "extended part: "
                "size 0x%x lba %d\n", size, start);
        }
    }

    kfree(msdos);
    return ret;
}

state msdos_match(struct block_device *bdev)
{
    struct block_part *entry, *next;
    struct msdos_head *msdos;
    state ret;
    int part;

    msdos = kzalloc(sizeof(*msdos), GFP_KERNEL);
    if (!msdos)
        return -ENOMEM;

    if ((ret = block_device_read(bdev, msdos, 0, 1))) {
        ret = -ENODATA;
        goto err_magic;
    }

    if (MSDOS_MAGIC != cpu_to_be16(msdos->magic)) {
        ret = -EINVAL;
        goto err_magic;
    }

    for (part = 0; part < 4; ++part) {
        uint32_t start, size;

        start = msdos->dpt[part].lba;
        size = msdos->dpt[part].size;

        if (!start || !size)
            continue;

        if (msdos_is_extended(msdos->dpt[part].type)) {
            if (msdos_scan_extended(bdev, start))
                goto err_scan;
        } else {
            entry = kzalloc(sizeof(*entry), GFP_KERNEL);
            if (!entry)
                goto err_scan;

            entry->start = start;
            entry->len = size;
            list_add_prev(&bdev->parts, &entry->list);
            dev_info(bdev->dev, "primary part: "
                "size 0x%x lba %d\n", size, start);
        }
    }

    return -ENOERR;

err_scan:
    list_for_each_entry_safe(entry, next, &bdev->parts, list) {
        list_del(&entry->list);
        kfree(entry);
    }
err_magic:
    kfree(msdos);
    return ret;
}

static struct partition_type msdos_part = {
    .name = "msdos",
    .match = msdos_match,
};

static state msdos_partition_init(void)
{
    return partition_register(&msdos_part);
}
fs_initcall(msdos_partition_init);
