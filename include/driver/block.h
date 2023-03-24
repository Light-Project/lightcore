/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_H_
#define _DRIVER_BLOCK_H_

#include <device.h>
#include <fsdev.h>

/**
 * enum block_request_type - request type for block device.
 * @read: read data from a block device.
 * @write: write data to a block device.
 * @flush: flush the cache of a block device.
 */
enum block_request_type {
    BLK_REQ_READ    = 0x01,
    BLK_REQ_WRITE   = 0x02,
    BLK_REQ_FLUSH   = 0x03,
};

/**
 * struct block_request - describes a block device request.
 * @list: the request chain of the block device.
 * @type: request type for block device.
 * @sector: starting number of the requested sector.
 * @length: number of sectors requested to be processed.
 * @buffer: buffer that provides read and write operations.
 */
struct block_request {
    struct list_head list;
    enum block_request_type type;
    sector_t sector;
    blkcnt_t length;
    void *buffer;
};

/**
 * struct block_part - describes a block device partition.
 * @fsdev: disk partition simulated as fsdev.
 * @device: the parent block for this partition.
 * @list: list node for managing block partition.
 * @start: start sector of this partition.
 * @len: total sector number of this partition.
 */
struct block_part {
    struct fsdev fsdev;
    struct block_device *device;
    struct list_head list;
    sector_t start;
    blkcnt_t len;
};

#define fsdev_to_block_part(fd) \
    container_of(fd, struct block_part, fsdev)

/**
 * struct block_device - describe a block device.
 * @dev: points to the parent device of the block device.
 * @parts: partitions owned by block device.
 * @ops: operations method of block device.
 * @pdata: private data of block device.
 */
struct block_device {
    struct device *dev;
    struct list_head parts;
    struct block_ops *ops;
    void *pdata;
};

/**
 * struct block_ops - describe the operations of a block device.
 * @start: start the operation of a block device.
 * @stop: stop the operation of a block device.
 * @enqueue: enqueue a request to block device.
 * @dequeue: dequeue a request from block device
 */
struct block_ops {
    state (*start)(struct block_device *bdev);
    state (*stop)(struct block_device *bdev);
    state (*enqueue)(struct block_device *bdev, struct block_request *req);
    state (*dequeue)(struct block_device *bdev, struct block_request *req);
};

extern spinlock_t block_lock;
extern struct list_head block_list;

extern state block_device_read(struct block_device *bdev, void *buff, sector_t sector, size_t len);
extern state block_add_fsdev(struct block_device *bdev);
extern state block_device_register(struct block_device *bdev);
extern void block_device_unregister(struct block_device *bdev);

#endif /* _DRIVER_BLOCK_H_ */
