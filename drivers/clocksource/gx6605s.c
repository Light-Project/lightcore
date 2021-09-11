/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-timer"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <irq.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/clocksource.h>
#include <driver/clocksource/gx6605s.h>
#include <printk.h>

#include <asm/io.h>

struct gx6605s_device {
    struct clocksource_device clk;
    void *mmio;
};

struct gx6605s_device root_intc;

static inline uint32_t
gx6605s_read(struct gx6605s_device *gdev, uint16_t reg)
{
    return readl(gdev->mmio + reg);
}

static inline void
gx6605s_write(struct gx6605s_device *gdev, uint16_t reg, uint32_t val)
{
    writel(gdev->mmio + reg, val);
}

static state gx6605s_probe(struct platform_device *pdev)
{
    return -ENOERR;
}

static const struct dt_device_id gx6605_device_id[] = {
    { .compatible = "csky,gx6605s-timer" },
    { }, /* NULL */
};

static struct platform_driver gx6605_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605_device_id,
    .probe = gx6605s_probe,
};

static state gx6605_init(void)
{
    return platform_driver_register(&gx6605_driver);
}
clocksource_initcall(gx6605_init);
