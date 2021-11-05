/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "generic-gpio"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/gpio.h>
#include <asm/io.h>

struct ggpio_device {
    struct gpio_device gpio;
    void *dat, *set, *clr;
    void *dirin, *dirout;
};

#define gpio_to_ggpio(ggpio) \
    container_of(ggpio, struct ggpio_device, gpio)

static bool ggpio_value_get(struct gpio_device *gdev, unsigned int port)
{
    struct ggpio_device *ggdev = gpio_to_ggpio(gdev);
    return readl(ggdev->dat) >> port & 0x01;
}

static void ggpio_value_set(struct gpio_device *gdev, unsigned int port, bool value)
{
    struct ggpio_device *ggdev = gpio_to_ggpio(gdev);
    uint32_t val = readl(ggdev->dirout) & ~(1 << port);
    writel(ggdev->dirout, val | (1 << port));
}

static enum gpio_direction ggpio_direction_get(struct gpio_device *gdev, unsigned int port)
{
    struct ggpio_device *ggdev = gpio_to_ggpio(gdev);
    return readl(ggdev->dirout) >> port & 0x01 ? GPIO_DIRECTION_OUTPUT : GPIO_DIRECTION_INPUT;
}

static void ggpio_direction_set(struct gpio_device *gdev, unsigned int port, enum gpio_direction dire)
{
    struct ggpio_device *ggdev = gpio_to_ggpio(gdev);
    uint32_t val = readl(ggdev->dirout) & ~(1 << port);
    writel(ggdev->dirout, val | ((dire == GPIO_DIRECTION_OUTPUT) << port));
}

static struct gpio_ops ggpio_ops = {
    .value_get = ggpio_value_get,
    .value_set = ggpio_value_set,
    .direction_get = ggpio_direction_get,
    .direction_set = ggpio_direction_set,
};

static state ggpio_probe(struct platform_device *pdev, void *pdata)
{
    struct ggpio_device *ggdev;
    struct resource *res;

    ggdev = dev_kzalloc(&pdev->dev, sizeof(*ggdev), GFP_KERNEL);
    if (!ggdev)
        return -ENOMEM;

    res = platform_name_resource(pdev, "dat");
    ggdev->dat = dev_ioremap(&pdev->dev, res->start, res->size);
    if (!ggdev->dat)
        return -ENOMEM;

    res = platform_name_resource(pdev, "dirout");
    ggdev->dirout = dev_ioremap(&pdev->dev, res->start, res->size);
    if (!ggdev->dirout)
        return -ENOMEM;

    res = platform_name_resource(pdev, "set");
    ggdev->dirout = dev_ioremap(&pdev->dev, res->start, res->size);
    if (!ggdev->dirout)
        return -ENOMEM;

    res = platform_name_resource(pdev, "clr");
    ggdev->dirout = dev_ioremap(&pdev->dev, res->start, res->size);
    if (!ggdev->dirout)
        return -ENOMEM;

    ggdev->gpio.ops = &ggpio_ops;
    return gpio_register(&ggdev->gpio);
}

static struct dt_device_id ggpio_ids[] = {
    { .compatible = "wd,mbl-gpio" },
    { }, /* NULL */
};

static struct platform_driver ggpio_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = ggpio_ids,
    .probe = ggpio_probe,
};

static state ggpio_init(void)
{
    return platform_driver_register(&ggpio_driver);
}
driver_initcall(ggpio_init);
