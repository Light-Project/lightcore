/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <kmalloc.h>
#include <initcall.h>
#include <driver/block.h>
#include <driver/block/partition.h>
#include <driver/block/efi.h>
#include <driver/block/msdos.h>
#include <lightcore/asm/byteorder.h>

static state efi_match(struct block_device *bdev)
{
    struct block_part *entry, *next;
    struct efi_head *head;
    struct efi_sector *sector;
    unsigned int max_part, part;
    state ret;

    head = kmalloc(sizeof(*head) + sizeof(*sector), GFP_KERNEL);
    sector = (void *)head + sizeof(*head);
    if (!head)
        return -ENOMEM;

    if ((ret = block_device_read(bdev, head, EFI_HEAD_LBA, 1))) {
        ret = -ENXIO;
        goto err_magic;
    }

    if (memcmp(EFI_MAGIC, &head->magic, sizeof(head->magic))) {
        ret = -ENODATA;
        goto err_magic;
    }

    max_part = le32_to_cpu(head->num_partition_entries);
    for (part = 0; part < max_part; ++part) {
        uint64_t start, end;

        if (align_check(part, 4) &&
           (ret = block_device_read(bdev, sector, EFI_SECTOR_LBA + (part / 4), 1))) {
            ret = -ENXIO;
            goto err_scan;
        }

        start = le64_to_cpu(sector->entry[part % 4].start_lba);
        end = le64_to_cpu(sector->entry[part % 4].end_lba);

        if (!start || !end)
            continue;

        entry = kzalloc(sizeof(*entry), GFP_KERNEL);
        if (!entry) {
            ret = -ENOMEM;
            goto err_scan;
        }

        entry->start = start;
        entry->len = end - start + 1;
        list_add_prev(&bdev->parts, &entry->list);
    }

    kfree(head);
    return -ENOERR;

err_scan:
    list_for_each_entry_safe(entry, next, &bdev->parts, list) {
        list_del(&entry->list);
        kfree(entry);
    }

err_magic:
    kfree(head);
    return ret;
}

static struct partition_type efi_part = {
    .name = "efi",
    .match = efi_match,
};

static state efi_partition_init(void)
{
    return partition_register(&efi_part);
}
fs_initcall(efi_partition_init);
