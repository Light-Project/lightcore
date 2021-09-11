/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FSDEV_H_
#define _FSDEV_H_

#include <types.h>
#include <state.h>
#include <mm/mm_types.h>

typedef enum request_type {
    REQ_READ    = 0x01,
    REQ_WRITE   = 0x02,
    REQ_FLUSH   = 0x03,
} request_t;

struct fsdev_request {
    enum request_type type;
    void *buffer;
    sector_t sector;
    blkcnt_t len;
};

struct fsdev {
    struct super_block *sb;
    size_t sector_size;
    struct fsdev_ops *ops;
    struct list_head list;
};

struct fsdev_ops {
    state (*request)(struct fsdev *, struct fsdev_request *);
};

state fsdev_page_read(struct fsdev *, struct page *, sector_t);
state fsdev_page_write(struct fsdev *, struct page *, sector_t);

state fsdev_register(struct fsdev *fsdev);
void fsdev_unregister(struct fsdev *fsdev);

#endif  /* _FSDEV_H_ */
