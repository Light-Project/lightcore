/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FSDEV_H_
#define _FSDEV_H_

#include <types.h>
#include <state.h>
#include <mm/types.h>

struct fsdev {
    struct list_head list;
    struct fsdev_ops *ops;
    size_t sector_nr;
    size_t sector_size;
};

struct fsdev_ops {
    state (*open)(struct fsdev *fsdev);
    void (*close)(struct fsdev *fsdev);
    state (*sync)(struct fsdev *fsdev);
    state (*read)(struct fsdev *fsdev, sector_t sector, void *buffer, size_t snr);
    state (*write)(struct fsdev *fsdev, sector_t sector, void *buffer, size_t snr);
};

state fsdev_page_read(struct fsdev *, sector_t, struct page *);
state fsdev_page_write(struct fsdev *, sector_t, struct page *);

state fsdev_read(struct fsdev *, unsigned long pos, void *buf, size_t len);
state fsdev_write(struct fsdev *, unsigned long pos, void *buf, size_t len);

state fsdev_register(struct fsdev *fsdev);
void fsdev_unregister(struct fsdev *fsdev);

#endif  /* _FSDEV_H_ */
