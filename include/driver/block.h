/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_H_
#define _DRIVER_BLOCK_H_

#include <device.h>
#include <fsdev.h>

enum request_type {
    REQ_READ    = 0x01,
    REQ_WRITE   = 0x02,
    REQ_FLUSH   = 0x03,
};

struct block_request {
    struct list_head list;
    enum request_type type;
    sector_t sector;
    unsigned int length;
    void *buffer;
};

struct block_part {
    struct fsdev fsdev;
    sector_t start, len;
    struct block_device *device;
    struct list_head list;
};

#define fsdev_to_block_part(fd) \
    container_of(fd, struct block_part, fsdev)

struct block_device {
    struct device *dev;
    struct list_head parts;
    struct block_ops *ops;
};

struct block_ops {
    state (*start)(struct block_device *);
    state (*stop)(struct block_device *);
    state (*enqueue)(struct block_device *, struct block_request *);
    state (*dequeue)(struct block_device *, struct block_request *);
};

extern state block_device_read(struct block_device *blk, void *buff, sector_t sector, size_t len);

extern state block_add_fsdev(struct block_device *);
extern state block_device_register(struct block_device *);
extern void block_device_unregister(struct block_device *);

#endif  /* _DRIVER_BLOCK_H_ */
