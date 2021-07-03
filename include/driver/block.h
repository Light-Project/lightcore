
#ifndef _DRIVER_BLOCK_H_
#define _DRIVER_BLOCK_H_

#include <state.h>
#include <fs.h>
// #include <driver/block/badblock.h>

struct block_device;

struct block_ops {
    state (*open)(struct block_device *, fmode_t);
    state (*release)(struct block_device *, fmode_t);
    state (*read)(struct block_device *, fmode_t);

};

#define BLOCK_NAME_LEN  32

struct block_device {
    char name[BLOCK_NAME_LEN];

    struct block_ops *ops;

};

state block_read(struct block_device *bdev);
state block_write(struct block_device *bdev);

state block_register(struct block_device *blk);

#endif