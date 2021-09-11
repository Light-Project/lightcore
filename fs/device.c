/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <fsdev.h>

LIST_HEAD(fsdev_list);

state fsdev_read(struct fsdev *dev, void *buffer, sector_t sector, blkcnt_t len)
{
    struct fsdev_request *request;

    request = kmalloc(sizeof(*request), GFP_KERNEL);
    if (!request)
        return -ENOMEM;

    request->type = REQ_READ;
    request->buffer = buffer;
    request->sector = sector;
    request->len = len;
    return dev->ops->request(dev, request);
}

state fsdev_register(struct fsdev *fsdev)
{
    list_add_prev(&fsdev_list, &fsdev->list);
    return -ENOERR;
}

void fsdev_unregister(struct fsdev *fsdev)
{

}

