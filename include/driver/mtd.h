/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_MTD_H_
#define _DRIVER_MTD_H_

#include <types.h>
#include <state.h>

struct mtd_device {

    uint32_t writesize;
    uint32_t erasesize;

    struct mtd_ops *ops;
};

struct mtd_ops {
    state (*read)(struct mtd_device *, loff_t pos, void *buf, uint64_t len);
    state (*write)(struct mtd_device *, loff_t pos, void *buf, uint64_t len);
    state (*erase)(struct mtd_device *, loff_t pos, uint64_t len);
	state (*suspend) (struct mtd_info *mtd);
	void (*resume) (struct mtd_info *mtd);
};

#endif  /* _DRIVER_MTD_H_ */
