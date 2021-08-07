/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <driver/block.h>
#include <driver/block/partition.h>

state block_device_read(struct block_device *blk, void *buff, 
                 sector_t sector, size_t len)
{
    struct block_request *request;

    request = kzalloc(sizeof(*request), GFP_KERNEL);
    if (!request)
        return -ENOMEM;

    request->type = BLOCK_REQ_READ;
    request->sector = sector;
    request->sector_nr = len;
    request->buffer = buff;

    return blk->ops->enqueue(blk, request);
}

state block_device_register(struct block_device *blk)
{

    block_add_parts(blk);

    return -ENOERR;
}
