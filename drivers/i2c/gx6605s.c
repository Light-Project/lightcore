/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-i2c"

#include <initcall.h>
#include <driver/platform.h>
#include <driver/i2c.h>
#include <driver/i2c/gx6605s.h>
#include <asm/io.h>

struct gx6605s_device {
    struct i2c_host host;
    void *base;
};

#define rtc_to_gx6605s(wdg) \
    container_of(wdg, struct gx6605s_device, rtc)

static __always_inline uint32_t
gx6605s_read(struct gx6605s_device *gdev, unsigned int reg)
{
    return readl(gdev->base + reg);
}

static __always_inline void
gx6605s_write(struct gx6605s_device *gdev, unsigned int reg, uint32_t val)
{
    writel(gdev->base + reg, val);
}

static struct dt_device_id gx6605s_i2c_ids[] = {
    { .compatible = "nationalchip,gx6605s-i2c" },
    { },  /* NULL */
};

static struct platform_driver gx6605s_i2c_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_i2c_ids,
    .probe = gx6605s_i2c_probe,
};

static state gx6605s_i2c_init(void)
{
    return platform_driver_register(&gx6605s_i2c_driver);
}
driver_initcall(gx6605s_i2c_init);
