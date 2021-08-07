/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_PARTITION_H_
#define _DRIVER_BLOCK_PARTITION_H_

#include <slist.h>

struct partition_table {
    sector_t start, len;
    struct partition_table *next;
};

struct partition_type {
    const char *name;
    struct slist_head list;
    struct partition_table *(*match)(struct block_device *);
};

state block_add_parts(struct block_device *bdev);
state partition_register(struct partition_type *);
void partition_unregister(struct partition_type *);

#endif  /* _DRIVER_BLOCK_PARTITION_H_ */
