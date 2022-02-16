/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <fsdev.h>
#include <export.h>

static LIST_HEAD(fsdev_list);

/**
 * fsdev_read - read data form a filesystem device
 * @fsdev: the filesystem device to read
 * @pos: read sector position
 * @buf: read buffer pointer
 * @len: read sector length
 */
state fsdev_read(struct fsdev *fsdev, unsigned long pos, void *buf, size_t len)
{
    size_t secs = fsdev->sector_size;

    if (align_check(pos, secs) && align_check(len, secs))
        return fsdev->ops->read(fsdev, pos / len, buf, len / secs);

    return -ENOERR;
}
EXPORT_SYMBOL(fsdev_read);

/**
 * fsdev_write - write data to a filesystem device
 * @fsdev: the filesystem device to write
 * @pos: write sector position
 * @buf: write buffer pointer
 * @len: write sector length
 */
state fsdev_write(struct fsdev *fsdev, unsigned long pos, void *buf, size_t len)
{
    size_t secs = fsdev->sector_size;

    if (align_check(pos, secs) && align_check(len, secs))
        return fsdev->ops->write(fsdev, pos / len, buf, len / secs);

    return -ENOERR;
};
EXPORT_SYMBOL(fsdev_write);

/**
 * fsdev_register - register a filesystem device
 * @fsdev: the filesystem device to register
 */
state fsdev_register(struct fsdev *fsdev)
{
    if (!fsdev->ops)
        return -EINVAL;

    list_add(&fsdev_list, &fsdev->list);
    return -ENOERR;
}
EXPORT_SYMBOL(fsdev_register);

/**
 * fsdev_unregister - unregister a filesystem device
 * @fsdev: the filesystem device to unregister
 */
void fsdev_unregister(struct fsdev *fsdev)
{
    list_del(&fsdev->list);
}
EXPORT_SYMBOL(fsdev_unregister);
