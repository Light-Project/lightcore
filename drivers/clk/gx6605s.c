/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-clk"

#include <initcall.h>
#include <driver/platform.h>
#include <driver/platform/gx6605s.h>
#include <dt-bindings/clk/gx6605s.h>
#include <driver/clk.h>
#include <driver/reset.h>

struct gx6605s_channel {
    struct clk_channel clk;
    uint16_t sel_reg;
    uint16_t div_reg;
    uint32_t sel_mask;
    uint32_t reset_mask, end_mask;
};

struct gx6605s_device {
    struct clk_device clk;
    struct irqchip_channel *irqchip;
    void *mmio;
};

#define clk_to_gdev(cdev) \
    container_of(cdev, struct gx6605s_device, clk)

static __always_inline uint32_t
gx6605s_read(struct gx6605s_device *gdev, uint16_t reg)
{
    return readl(gdev->mmio + reg);
}

static __always_inline void
gx6605s_write(struct gx6605s_device *gdev, int reg, uint32_t val)
{
    writel(gdev->mmio + reg, val);
}

static struct gx6605s_channel gx6605s_channels[] = {
    [GX6605S_CLOCK_AUDIO] = {
        .clk.name   = "audio",
        .sel_reg    = GX6605S_SOURCE_SEL1,
        .div_reg    = GX6605S_CLOCK_DIV_CONFIG1,
        .sel_mask   = GX6605S_SOURCE_SEL1_AUDIO,
        .end_mask   = GX6605S_CLOCK_DIV1_AUDIO_END,
        .reset_mask = GX6605S_CLOCK_DIV1_AUDIO_RST,
    },
    [GX6605S_CLOCK_UART] = {
        .clk.name   = "uart",
        .sel_reg    = GX6605S_SOURCE_SEL1,
        .div_reg    = GX6605S_CLOCK_DIV_CONFIG1,
        .sel_mask   = GX6605S_SOURCE_SEL1_AUDIO,
        .end_mask   = GX6605S_CLOCK_DIV1_AUDIO_END,
        .reset_mask = GX6605S_CLOCK_DIV1_AUDIO_RST,
    },
};

static state gx6605s_clk_rate_get()
{

}

static struct clk_ops gx6605s_clk_ops = {
    .rate_get = ,
};

static state gx6605s_clk_probe(struct platform_device *pdev, const void *pdata)
{


    return -ENOERR;
}

static struct dt_device_id gx6605s_clk_ids[] = {
    { .compatible = "nationalchip,gx6605s-clk", },
    { }, /* NULL */
};

static struct platform_driver gx6605s_clk_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_clk_ids,
    .probe = gx6605s_clk_probe,
};

static state gx6605s_clk_init(void)
{
    return platform_driver_register(&gx6605s_clk_driver);
}
driver_initcall(gx6605s_clk_init);
