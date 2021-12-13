/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "pl11x-display"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <sleep.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/video.h>
#include <driver/video/pl11x.h>

#include <asm/io.h>

struct pl11x_device {
    struct video_device video;
    uint8_t cntl_offset;
    void *base;
};

#define video_to_pl11x(vdev) \
    container_of(vdev, struct pl11x_device, video)

static __always_inline uint32_t
pl11x_read(struct pl11x_device *pl11x, unsigned int reg)
{
    return readl(pl11x->base + reg);
}

static __always_inline void
pl11x_write(struct pl11x_device *pl11x, unsigned int reg, uint32_t val)
{
    writel(pl11x->base + reg, val);
}

static inline void pl11x_enable(struct pl11x_device *pl11x)
{
    uint32_t val;

    /* enable pl11x module first */
    val = pl11x_read(pl11x, pl11x->cntl_offset);
    val |= PL11X_CNTL_LCDEN;
    pl11x_write(pl11x, pl11x->cntl_offset, val);

    msleep(20);

    /* and then turn on pl11x's power */
    val |= PL11X_CNTL_LCDPWR;
    pl11x_write(pl11x, pl11x->cntl_offset, val);
}

static inline void pl11x_disable(struct pl11x_device *pl11x)
{
    uint32_t val;

    val = pl11x_read(pl11x, pl11x->cntl_offset);
    val &= ~PL11X_CNTL_LCDPWR;
    pl11x_write(pl11x, pl11x->cntl_offset, val);

    msleep(20);

    val &= ~PL11X_CNTL_LCDEN;
    pl11x_write(pl11x, pl11x->cntl_offset, val);
}

static struct video_ops pl11x_ops = {

};

static state pl11x_hw_init(struct pl11x_device *pl11x)
{

    pl11x_enable(pl11x);

    return -ENOERR;
}

static state pl11x_probe(struct platform_device *pdev, const void *pdata)
{
    struct pl11x_device *pl11x;
    resource_size_t start, size;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -ENODEV;

    pl11x = dev_kzalloc(&pdev->dev, sizeof(*pl11x), GFP_KERNEL);
    if (!pl11x)
        return -ENOMEM;
    platform_set_devdata(pdev, pl11x);

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    pl11x->base = dev_ioremap(&pdev->dev, start, size);
    if (!pl11x->base)
        return -ENOMEM;

    pl11x_hw_init(pl11x);

    pl11x->video.ops = &pl11x_ops;
    return video_register(&pl11x->video);
}

static struct dt_device_id pl11x_ids[] = {
    { .compatible = "arm,pl110" },
    { .compatible = "arm,pl111" },
    { }, /* NULL */
};

static struct platform_driver pl11x_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = pl11x_ids,
    .probe = pl11x_probe,
};

static state pl11x_init(void)
{
    return platform_driver_register(&pl11x_driver);
}
driver_initcall(pl11x_init);
