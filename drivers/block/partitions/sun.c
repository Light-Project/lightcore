/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "sun-part"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <device.h>
#include <printk.h>
#include <driver/block.h>
#include <driver/block/partition.h>
#include <driver/block/sun.h>
#include <lightcore/asm/byteorder.h>

static state sun_match(struct block_device *bdev)
{
    struct block_part *entry, *next;
    struct sun_head *sun;
    unsigned int max_part, part;
    uint32_t spc;
    bool use_vtoc;
    state ret;

    sun = kmalloc(sizeof(*sun), GFP_KERNEL);
    if (!sun)
        return -ENOMEM;

    if ((ret = block_device_read(bdev, sun, 0, 1))) {
        ret = -ENXIO;
        goto err_magic;
    }

    if (SUN_LABEL_MAGIC != be16_to_cpu(sun->magic)) {
        ret = -ENODATA;
        goto err_magic;
    }

    use_vtoc = ((be32_to_cpu(sun->vtoc.sanity) == SUN_VTOC_SANITY) &&
                (be32_to_cpu(sun->vtoc.version) == 1) &&
                (be16_to_cpu(sun->vtoc.nparts) <= 8));

    max_part = use_vtoc ? be16_to_cpu(sun->vtoc.nparts) : 8;

    use_vtoc = use_vtoc || !(sun->vtoc.sanity ||
               sun->vtoc.version || sun->vtoc.nparts);

	spc = be16_to_cpu(sun->ntrks) * be16_to_cpu(sun->nsect);

    for (part = 0; part < max_part; ++part) {
        uint32_t start, size;

        start = be32_to_cpu(sun->part[part].start_cylinder) * spc;
        size = be32_to_cpu(sun->part[part].num_sectors);

        if (!start || !size)
            continue;

        entry = kzalloc(sizeof(*entry), GFP_KERNEL);
        if (!entry) {
            ret = -ENOMEM;
            goto err_scan;
        }

        entry->start = start;
        entry->len = size;

        list_add_prev(&bdev->parts, &entry->list);
        dev_info(bdev->dev, "part%d: size %lld lba %lld\n",
            part, entry->len, entry->start);
    }

    kfree(sun);
    return -ENOERR;

err_scan:
    list_for_each_entry_safe(entry, next, &bdev->parts, list) {
        list_del(&entry->list);
        kfree(entry);
    }

err_magic:
    kfree(sun);
    return ret;
}

static struct partition_type sun_part = {
    .name = "sun",
    .match = sun_match,
};

static state msdos_partition_init(void)
{
    return partition_register(&sun_part);
}
fs_initcall(msdos_partition_init);

