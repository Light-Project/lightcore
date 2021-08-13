/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <export.h>
#include <driver/block.h>
#include <driver/block/partition.h>

static SLIST_HEAD(partition_list);

state partition_scan(struct block_device *bdev)
{
    struct partition_type *part;
    state ret;

    slist_for_each_entry(part, &partition_list, list) {
        ret = part->match(bdev);
        if (!ret)
            return -ENOERR;
    }
    return ret;
}

state block_add_parts(struct block_device *bdev)
{
    struct partition_entry *part;

    list_head_init(&bdev->parts);
    partition_scan(bdev);

    list_for_each_entry(part, &bdev->parts, list) {
        
    }

    return -ENOERR;
}

state partition_register(struct partition_type *part)
{
    if(!part)
        return -EINVAL;
        
    slist_add(&partition_list, &part->list);
    return -ENOERR;
}
EXPORT_SYMBOL(partition_register);

void partition_unregister(struct partition_type *part)
{
    if(!part)
        return;

    slist_del(&partition_list, &part->list);
}
EXPORT_SYMBOL(partition_unregister);
