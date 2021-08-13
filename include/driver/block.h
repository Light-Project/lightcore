/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_H_
#define _DRIVER_BLOCK_H_

#include <types.h>
#include <state.h>
#include <list.h>

struct block_device;

enum block_request_type {
    BLOCK_REQ_READ,
    BLOCK_REQ_WRITE,
    BLOCK_REQ_FLUSH,
};

struct block_request {
    struct list_head list;          /* request list */
    enum block_request_type type;   /* request type */

    sector_t sector;
    size_t sector_nr;
    void *buffer;
};

struct block_ops {
    state (*start)(struct block_device *);
    state (*stop)(struct block_device *);
    state (*enqueue)(struct block_device *, struct block_request *);
    state (*dequeue)(struct block_device *, struct block_request *);
};

struct block_device {
    struct block_ops *ops;
    struct list_head parts;
};

/* block_part.c */

/* core.c */
state block_device_register(struct block_device *);
state block_device_read(struct block_device *blk, void *buff, sector_t sector, size_t len);

#endif  /* _DRIVER_BLOCK_H_ */
