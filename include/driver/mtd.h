/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_MTD_H_
#define _DRIVER_MTD_H_

#include <device.h>
#include <fsdev.h>

enum mtd_type {
    MTD_ROM,
    MTD_RAM,
    MTD_NORFLASH,
    MTD_NANDFLASH,
};

struct mtd_part {
    struct fsdev fsdev;
    struct mtd_device *device;
    struct list_head list;
    const char *name;
    uint64_t offset;
    uint64_t size;
};

#define fsdev_to_mtd(fd) \
    container_of(fd, struct mtd_part, fsdev)

struct mtd_device {
    struct device *dev;
    struct list_head list;
    struct list_head parts;
    uint64_t size;
    uint32_t writesize;
    uint32_t erasesize;
    struct mtd_ops *ops;
};

struct mtd_ops {
    state (*read)(struct mtd_device *mtd, loff_t pos, void *buf, uint64_t len, uint64_t *retlen);
    state (*write)(struct mtd_device *mtd, loff_t pos, void *buf, uint64_t len, uint64_t *retlen);
    state (*erase)(struct mtd_device *mtd, loff_t pos, uint64_t len, uint64_t *retlen);
    state (*suspend)(struct mtd_device *mtd);
    void (*resume)(struct mtd_device *mtd);
    void (*reboot)(struct mtd_device *mtd);
};

extern spinlock_t mtd_lock;
extern struct list_head mtd_list;

extern state mtd_fsdev_register(struct mtd_device *mdev);
extern void mtd_fsdev_unregister(struct mtd_device *mdev);

extern state mtd_register(struct mtd_device *mdev, struct mtd_part *part, unsigned int pnr);
extern void mtd_unregister(struct mtd_device *mdev);

#endif  /* _DRIVER_MTD_H_ */
