/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i8237"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <kmalloc.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/dma.h>
#include <driver/dma/i8237.h>
#include <printk.h>
#include <asm/io.h>

struct i8237_device {
    struct dma_device dma;
    resource_size_t base;
    resource_size_t page[4];
    spinlock_t lock;
};

#define dma_to_idev(ddev) \
    container_of(ddev, struct i8237_device, dma)

static __always_inline void
i8237_out(struct i8237_device *idev, unsigned int reg, uint8_t val)
{
    outb(idev->base + reg, val);
}

static state i8237_channel_set(struct i8237_device *idev, unsigned int index,
                               bool dire, dma_addr_t addr, dma_addr_t count)
{
    dma_addr_t end = addr + count;

    if ((addr >> 16) != (end >> 16))
        return -EADDRNOTAVAIL;

    i8237_out(idev, I8237_SMASK, I8237_SMASK_MASKON | (I8237_SMASK_MASK & index));
    i8237_out(idev, I8237_TRIGGER, 0x00);
    i8237_out(idev, I8237_CH_SAR(index), addr);
    i8237_out(idev, I8237_CH_SAR(index), addr >> 8);
    i8237_out(idev, I8237_TRIGGER, 0x00);
    i8237_out(idev, I8237_CH_CR(index), count);
    i8237_out(idev, I8237_CH_CR(index), count >> 8);

    i8237_out(idev, I8237_MODE, I8237_MODE_MOD_SINGLE |
              (dire ? I8237_MODE_TRA_TOMEM : I8237_MODE_TRA_TOPER) |
              (I8237_MODE_SEL_MASK & index));

    outb(idev->page[index], addr >> 16);
    i8237_out(idev, I8237_SMASK, I8237_SMASK_MASK & index);

    return -ENOERR;
}

static struct dma_ops i8237_ops = {
};

static state i8237_prebe(struct platform_device *pdev, void *pdata)
{
    return -ENOERR;
}

static struct dt_device_id i8237_id_table[] = {
    { .compatible = "intel,i8237" },
    { }, /* NULL */
};

static struct platform_driver i8237_driver {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i8237_id_table,
    .probe = i8237_prebe,
};

static state i8237_init(void)
{
    return platform_driver_register(&i8237_driver);
}
arch_initcall(i8237_init);
