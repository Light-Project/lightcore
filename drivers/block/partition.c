/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <export.h>
#include <driver/block.h>
#include <driver/block/partition.h>

static SLIST_HEAD(partition_list);

struct partition_table *
partition_scan(struct block_device *bdev)
{
    struct partition_type *part;
    struct partition_table *entry;

    slist_for_each_entry(part, &partition_list, list) {
        entry = part->match(bdev);
        if(entry)
            return entry;
    }

    return NULL;
}

state block_add_one(struct partition_table *part, 
                    struct block_device *bdev)
{

    return -ENOERR;
}

state block_add_parts(struct block_device *bdev)
{
    struct partition_table *part;

    part = partition_scan(bdev);

    while (part) {
        block_add_one(part, bdev);
        part = part->next;
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
