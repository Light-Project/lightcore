/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "msdos-part"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <device.h>
#include <printk.h>
#include <driver/block.h>
#include <driver/block/partition.h>
#include <driver/block/msdos.h>
#include <driver/block/efi.h>
#include <lightcore/asm/byteorder.h>

static inline bool msdos_is_extended(uint8_t type)
{
    return (type == MSDOS_DOS_EXT_PART   ||
            type == MSDOS_WIN98_EXT_PART ||
            type == MSDOS_LINUX_EXT_PART);
}

static state msdos_scan_extended(struct block_device *bdev, uint32_t base)
{
    struct block_part *entry;
    struct msdos_head *msdos;
    uint32_t pos = base;
    int count, part;
    state ret;

    msdos = kmalloc(sizeof(*msdos), GFP_KERNEL);
    if (!msdos)
        return -ENOMEM;

    for (count = 0; count < 100; ++count) {

        if ((ret = block_device_read(bdev, msdos, pos, 1)))
            break;

        if (MSDOS_MAGIC != be16_to_cpu(msdos->magic))
            break;

        for (part = 0; part < 4; ++part) {
            uint32_t start, size;

            start = le32_to_cpu(msdos->dpt[part].lba);
            size = le32_to_cpu(msdos->dpt[part].size);

            if (!start || !size)
                goto finish;

            if (msdos_is_extended(msdos->dpt[part].type)) {
                pos = base + start;
                break;
            }

            entry = kzalloc(sizeof(*entry), GFP_KERNEL);
            if (!entry) {
                ret = -ENOMEM;
                goto finish;
            }

            entry->start = start + pos;
            entry->len = size;

            list_add_prev(&bdev->parts, &entry->list);
            dev_info(bdev->dev, "extended part%d.%d: size %lld lba %lld\n",
                count, part, entry->len, entry->start);
        }
    }

finish:
    kfree(msdos);
    return ret;
}

static state msdos_match(struct block_device *bdev)
{
    struct block_part *entry, *next;
    struct msdos_head *msdos;
    struct efi_head *efi;
    unsigned int part;
    state ret;

    msdos = kmalloc(sizeof(*msdos) + sizeof(*msdos), GFP_KERNEL);
    efi = (void *)msdos + sizeof(*msdos);
    if (!msdos)
        return -ENOMEM;

    if ((ret = block_device_read(bdev, msdos, 0, 2))) {
        ret = -ENXIO;
        goto err_magic;
    }

    if (MSDOS_MAGIC != be16_to_cpu(msdos->magic)) {
        ret = -ENODATA;
        goto err_magic;
    }

    if (!memcmp(EFI_MAGIC, &efi->magic, sizeof(efi->magic))) {
        ret = -ENODATA;
        goto err_magic;
    }

    for (part = 0; part < 4; ++part) {
        uint32_t start, size;

        start = le32_to_cpu(msdos->dpt[part].lba);
        size = le32_to_cpu(msdos->dpt[part].size);

        if (!start || !size)
            continue;

        if (msdos_is_extended(msdos->dpt[part].type)) {
            if (msdos_scan_extended(bdev, start))
                goto err_scan;
        } else {
            entry = kzalloc(sizeof(*entry), GFP_KERNEL);
            if (!entry) {
                ret = -ENOMEM;
                goto err_scan;
            }

            entry->start = start;
            entry->len = size;

            list_add_prev(&bdev->parts, &entry->list);
            dev_info(bdev->dev, "primary part%d: size %lld lba %lld\n",
                part, entry->len, entry->start);
        }
    }

    kfree(msdos);
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
