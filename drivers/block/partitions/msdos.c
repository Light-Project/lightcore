/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "msdos-part: " fmt

#include <mm.h>
#include <init/initcall.h>
#include <driver/block.h>
#include <driver/block/partition.h>
#include <driver/block/msdos.h>
#include <printk.h>

#include <lightcore/asm/byteorder.h>

static state msdos_extended(struct block_device *bdev, uint32_t start)
{
    struct msdos_head *msdos;
    int count, part;

    for (count = 0; count < 100; count++) {
        msdos = kmalloc(sizeof(*msdos), GFP_KERNEL);
        if (!msdos)
            return -ENOMEM;
        
        block_device_read(bdev, msdos, 0, 1);
        if (MSDOS_MAGIC != cpu_to_be16(msdos->magic))
            goto fail;

        for (part = 0; part < 4; ++part) {

        
        }

        kfree(msdos);
    }

fail:
    kfree(msdos);
    return -EINVAL;
}

static struct partition_table *
msdos_match(struct block_device *bdev)
{
    struct msdos_head *msdos;
    int part;

    msdos = kmalloc(sizeof(*msdos), GFP_KERNEL);
    if (!msdos)
        return NULL;
        
    block_device_read(bdev, msdos, 0, 1);
    if (MSDOS_MAGIC != cpu_to_be16(msdos->magic))
        return NULL;

    for (part = 0; part < 4; ++part) {
        uint32_t start, size;

        start = msdos->dpt[part].lba;
        size = msdos->dpt[part].size;

        if (!start)
            continue;

        if (msdos->dpt[part].size != MSDOS_DOS_EXT_PART   ||
            msdos->dpt[part].size != MSDOS_WIN98_EXT_PART ||
            msdos->dpt[part].size != MSDOS_LINUX_EXT_PART) {

        } else  /* extended partition */
            msdos_extended(bdev, start + size);
    }

    return NULL;
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
