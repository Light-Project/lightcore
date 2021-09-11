/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <fsdev.h>
#include <driver/block.h>
#include <driver/block/partition.h>

// static state block_fsdev_request(struct fsdev *fsdev, struct fsdev_request *req)
// {
//     struct block_part *part = fsdev_to_block_part(fsdev);
//     struct block_device *blk = part->device;
//     struct block_request *request;

//     request = kzalloc(sizeof(*request), GFP_KERNEL);
//     if (!request)
//         return -ENOMEM;

//     request->type = req;
//     request->sector = part->start + sector;
//     request->buffer = page_to_va(page);

//     return blk->ops->enqueue(blk, request);
// }

static struct fsdev_ops block_fsdev_ops = {
    // .request = block_fsdev_request,
};

state block_add_fsdev(struct block_device *bdev)
{
    struct block_part *part;
    state retval;

    list_for_each_entry(part, &bdev->parts, list) {
        struct fsdev *fsdev = &part->fsdev;

        fsdev->ops = &block_fsdev_ops;
        retval = fsdev_register(fsdev);
        if (retval)
            return retval;
    }

    return -ENOERR;
}