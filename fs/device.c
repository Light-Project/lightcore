/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <fsdev.h>

LIST_HEAD(fsdev_list);

state fsdev_read(struct fsdev *fsdev, unsigned long pos, void *buf, size_t len)
{
    size_t secs = fsdev->sector_size;

    if (aligned(pos, secs) && aligned(len, secs))
        return fsdev->ops->read(fsdev, pos / len, buf, len / secs);

    return -ENOERR;
}

state fsdev_write(struct fsdev *fsdev, unsigned long pos, void *buf, size_t len)
{
    size_t secs = fsdev->sector_size;

    if (aligned(pos, secs) && aligned(len, secs))
        return fsdev->ops->write(fsdev, pos / len, buf, len / secs);

    return -ENOERR;
};

state fsdev_register(struct fsdev *fsdev)
{
    if (!fsdev->ops)
        return -EINVAL;

    list_add(&fsdev_list, &fsdev->list);
    return -ENOERR;
}

void fsdev_unregister(struct fsdev *fsdev)
{
    list_del(&fsdev->list);
}

