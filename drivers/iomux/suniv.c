/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/platform.h>
#include <driver/iomux.h>
#include <driver/gpio/sunxi.h>
#include <asm/io.h>

struct suniv_device {
    struct iomux_device iomux;
    void *base;
};

#define iomux_to_suniv(idev) \
    container_of(idev, struct suniv_device, iomux)

struct

static struct iomux_ops suniv_iomux_ops = {
};

static state suniv_iomux_probe(struct platform_device *pdev, void *pdata)
{
    struct suniv_device *suniv;

    suniv = dev_kzalloc(&pdev->dev, sizeof(*suniv), GFP_KERNEL);
    if (!suniv)
        return -ENOMEM;

    suniv->iomux.ops = &suniv_iomux_ops;
    return -ENOERR;
}

static struct dt_device_id suniv_iomux_ids [] = {
    { .compatible = "allwinner,suniv-iomux" },
};

static struct platform_driver suniv_iomux_driver = {
    .dt_table = suniv_iomux_ids,
    .probe = suniv_iomux_probe,
};

static state suniv_iomux_init(void)
{
    return platform_driver_register(&suniv_iomux_driver);
}
driver_initcall(suniv_iomux_init);
