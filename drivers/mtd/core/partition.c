/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/mtd.h>

state mtd_partition_add(struct mtd_device *mdev, struct mtd_part *part, unsigned int pnr)
{
    unsigned int count;

    for (count = 0; count < pnr; ++count) {


    }


    return -ENOERR;
}
