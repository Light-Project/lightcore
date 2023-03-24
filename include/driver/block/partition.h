/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_PARTITION_H_
#define _DRIVER_BLOCK_PARTITION_H_

#include <state.h>
#include <list.h>

struct partition_type {
    const char *name;
    struct list_head list;
    state (*match)(struct block_device *);
};

extern state partition_scan(struct block_device *);
extern state partition_register(struct partition_type *);
extern void partition_unregister(struct partition_type *);

#endif /* _DRIVER_BLOCK_PARTITION_H_ */
