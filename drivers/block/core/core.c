/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "blk"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kmalloc.h>
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

    request->type = BLK_REQ_READ;
    request->sector = sector;
    request->length = len;
    request->buffer = buff;

    return blk->ops->enqueue(blk, request);
}

state block_device_register(struct block_device *blk)
{
    state ret;

    if (!blk->ops || !blk->dev)
        return -ENXIO;

    list_head_init(&blk->parts);

    if ((ret = partition_scan(blk))) {
        dev_debug(blk->dev, "partition table not found\n");
        return ret;
    }

    block_add_fsdev(blk);
    return -ENOERR;
}
