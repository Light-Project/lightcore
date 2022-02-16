/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <fsdev.h>
#include <driver/mtd.h>

state mtd_fsdev_read(struct fsdev *fsdev, sector_t sector, void *buffer, size_t snr)
{
    struct mtd_part *part = fsdev_to_mtd(fsdev);
    struct mtd_device *mdev = part->device;
    struct mtd_ops *ops = mdev->ops;
    uint64_t len;

    if (!ops->read)
        return -EOPNOTSUPP;

    ops->read(mdev, sector, buffer, snr, &len);
    return -ENOERR;
}

state mtd_fsdev_write(struct fsdev *fsdev, sector_t sector, void *buffer, size_t snr)
{
    struct mtd_part *part = fsdev_to_mtd(fsdev);
    struct mtd_device *mdev = part->device;
    struct mtd_ops *ops = mdev->ops;
    uint64_t len;

    if (!ops->write)
        return -EOPNOTSUPP;

    ops->write(mdev, sector, buffer, snr, &len);
    return -ENOERR;
}

static struct fsdev_ops block_fsdev_ops = {
    .read = mtd_fsdev_read,
    .write = mtd_fsdev_write,
};

state mtd_fsdev_register(struct mtd_device *mdev)
{
    struct mtd_part *part;
    state retval;

    list_for_each_entry(part, &mdev->parts, list) {
        struct fsdev *fsdev = &part->fsdev;

        fsdev->sector_size = mdev->writesize;
        fsdev->ops = &block_fsdev_ops;

        if ((retval = fsdev_register(fsdev)))
            return retval;
    }

    return -ENOERR;
}