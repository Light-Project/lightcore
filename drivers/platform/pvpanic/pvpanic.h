/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PVPANIC_H_
#define _PVPANIC_H_

#include <bits.h>
#include <device.h>

struct pvpanic_device {
    struct list_head list;
    struct device *dev;
    void *base;
    unsigned long capability;
    unsigned long events;
};

#define PVPANIC_PANICKED        BIT(0)
#define PVPANIC_CRASH_LOADED    BIT(1)

extern state pvpanic_probe(struct pvpanic_device *pdev);
extern void pvpanic_remove(struct pvpanic_device *pdev);

#endif  /* _PVPANIC_H_ */
