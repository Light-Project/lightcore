/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-timer"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <irq.h>
#include <timer.h>
#include <driver/irqchip.h>
#include <driver/platform.h>
#include <driver/clocksource.h>
#include <driver/clocksource/gx6605s.h>
#include <asm/io.h>

struct gx6605s_device {
    struct clocksource_device clk;
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

static void gx6605s_clksrc_clear(struct gx6605s_device *gdev)
{
    gx6605s_write(gdev, GX6605S_TIM_STATUS, GX6605S_TIM_STATUS_CLR);
}

irqreturn_t gx6605s_clksrc_handle(irqnr_t vector, void *data)
{
    struct gx6605s_device *gdev = data;

    gx6605s_clksrc_clear(gdev);
    timer_tick();

    return IRQ_RET_HANDLED;
}

static inline void gx6605s_clksrc_hw_init(struct gx6605s_device *gdev)
{
    uint32_t val;

    /* reset the timer at first */
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_RST);
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, 0);

    gx6605s_write(gdev, GX6605S_TIM_CLKDIV, 53);

    val = UINT32_MAX - (1000000 / CONFIG_SYSTICK_FREQ) + 1;
    gx6605s_write(gdev, GX6605S_TIM_RELOAD, val);

    /* start the timer */
    gx6605s_write(gdev, GX6605S_TIM_CONFIG, GX6605S_TIM_CONFIG_IRQ_EN | GX6605S_TIM_CONFIG_EN);
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_START);
}

static inline void gx6605s_clksrc_hw_deinit(struct gx6605s_device *gdev)
{
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, 0);
    gx6605s_write(gdev, GX6605S_TIM_CONFIG, 0);
}

static state gx6605s_clksrc_probe(struct platform_device *pdev, const void *pdata)
{
    struct gx6605s_device *gdev;
    resource_size_t start, size;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -EINVAL;

    gdev = dev_kzalloc(&pdev->dev, sizeof(*gdev), GFP_KERNEL);
    if (!gdev)
        return -ENOMEM;
    platform_set_devdata(pdev, gdev);

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);

    gdev->mmio = dev_ioremap(&pdev->dev, start, size);
    if (!gdev->mmio)
        return -ENOMEM;

    /* request irq handle */
    irq_request(10, 0, gx6605s_clksrc_handle, gdev, DRIVER_NAME);

    gdev->irqchip = dt_irqchip_channel(pdev->dt_node, 0);
    if (!gdev->irqchip) {
        dev_err(&pdev->dev, "unable to request irq\n");
        return -ENODEV;
    }
    irqchip_pass(gdev->irqchip);

    gx6605s_clksrc_hw_init(gdev);
    return -ENOERR;
}

static const struct dt_device_id gx6605s_clksrc_ids[] = {
    { .compatible = "nationalchip,gx6605s-timer" },
    { }, /* NULL */
};

static struct platform_driver gx6605_clksrc_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_clksrc_ids,
    .probe = gx6605s_clksrc_probe,
};

static state gx6605_clksrc_init(void)
{
    return platform_driver_register(&gx6605_clksrc_driver);
}
clocksource_initcall(gx6605_clksrc_init);
