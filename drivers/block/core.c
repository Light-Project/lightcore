/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "blk: " fmt

#include <mm.h>
#include <driver/block.h>
#include <driver/block/partition.h>
#include <printk.h>

state block_device_read(struct block_device *blk, void *buff,
                 sector_t sector, size_t len)
{
    struct block_request *request;

    request = kzalloc(sizeof(*request), GFP_KERNEL);
    if (!request)
        return -ENOMEM;

    request->type = REQ_READ;
    request->sector = sector;
    request->sector_nr = len;
    request->buffer = buff;

    return blk->ops->enqueue(blk, request);
}

state block_device_register(struct block_device *blk)
{
    state ret;

    if (!blk || !blk->ops)
        return -ENXIO;

    list_head_init(&blk->parts);

    ret = partition_scan(blk);
    if (ret != true) {
        pr_debug("Partition table not found on\n");
        return -ENODATA;
    }

    block_add_fsdev(blk);
    return -ENOERR;
}
