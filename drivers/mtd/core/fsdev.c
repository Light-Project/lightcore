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


}

state mtd_fsdev_write(struct fsdev *fsdev, sector_t sector, void *buffer, size_t snr)
{
    struct mtd_part *part = fsdev_to_mtd(fsdev);

}

static struct fsdev_ops block_fsdev_ops = {
    .read =
};

state mtd_fsdev_register(struct mtd_device *mdev)
{
    struct mtd_part *part;
    state retval;

    list_for_each_entry(part, &mdev->parts, list) {
        struct fsdev *fsdev = &part->fsdev;

        fsdev->write_size = mdev->writesize;
        fsdev->ops = &block_fsdev_ops;

        if ((retval = fsdev_register(fsdev)))
            return retval;
    }

    return -ENOERR;
}