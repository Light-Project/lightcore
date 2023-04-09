/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <driver/block.h>
#include <driver/block/partition.h>
#include <driver/block/mac.h>
#include <lightcore/asm/byteorder.h>

static state mac_match(struct block_device *bdev)
{
    struct block_part *entry, *next;
    struct mac_driver *driver;
    struct mac_partition *part;
    unsigned int secsize, datasize, offset;
    int map_blocks, slot;
    void *buffer;
    state ret;

    driver = kmalloc(1024, GFP_KERNEL);
    buffer = (void *)driver + 512;
    if (!driver)
        return -ENOMEM;

    if ((ret = block_device_read(bdev, driver, 0, 1))) {
        ret = -ENXIO;
        goto err_magic;
    }

    if (be16_to_cpu(driver->signature) != MAC_DRIVER_MAGIC) {
        ret = -ENODATA;
        goto err_magic;
    }

    secsize = be16_to_cpu(driver->block_size);
    datasize = round_down(secsize, 512);
    if ((ret = block_device_read(bdev, buffer, datasize / 512, 1))) {
        ret = -ENXIO;
        goto err_magic;
    }

    offset = secsize % 512;
    if (offset + sizeof(*part) > datasize) {
        ret = -ENODATA;
        goto err_magic;
    }

    part = buffer + offset;
    if (be16_to_cpu(part->signature) != MAC_PARTITION_MAGIC) {
        ret = -ENODATA;
        goto err_magic;
    }

    map_blocks = be32_to_cpu(part->map_count);
    if (map_blocks < 0) {
        ret = -ENODATA;
        goto err_magic;
    }

    for (slot = 1; slot < map_blocks; ++slot) {
        int pos = slot * secsize;

        if ((ret = block_device_read(bdev, buffer, pos / 512, 1))) {
            ret = -ENXIO;
            goto err_scan;
        }

		part = buffer + pos % 512;
		if (be16_to_cpu(part->signature) != MAC_PARTITION_MAGIC)
			break;

        entry = kzalloc(sizeof(*entry), GFP_KERNEL);
        if (!entry) {
            ret = -ENOMEM;
            goto err_scan;
        }

        entry->start = be32_to_cpu(part->start_block) * (secsize / 512);
        entry->len = be32_to_cpu(part->block_count) * (secsize / 512);
        list_add_prev(&bdev->parts, &entry->list);
    }

    kfree(driver);
    return -ENOERR;

err_scan:
    list_for_each_entry_safe(entry, next, &bdev->parts, list) {
        list_del(&entry->list);
        kfree(entry);
    }

err_magic:
    kfree(driver);
    return ret;
}
static struct partition_type mac_part = {
    .name = "mac",
    .match = mac_match,
};

static state mac_partition_init(void)
{
    return partition_register(&mac_part);
}
fs_initcall(mac_partition_init);
