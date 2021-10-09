/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_DEVRES_H_
#define _DEVICE_DEVRES_H_

#include <types.h>
#include <mm/gfp.h>

struct device;

void *dev_kmalloc(struct device *dev, size_t size, gfp_t gfp);
void dev_kfree(struct device *dev, void *block);
void *dev_ioremap(struct device *dev, phys_addr_t addr, size_t size);
void dev_iounmap(struct device *dev, void *block);

#define dev_kzalloc(dev, size, gfp) dev_kmalloc(dev, size, (gfp) | GFP_ZERO)
void devres_release_all(struct device *dev);

#endif  /* _DEVICE_DEVRES_H_ */
