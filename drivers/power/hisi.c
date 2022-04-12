/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "hisi-power"

#include <kmalloc.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/power.h>
#include <delay.h>
#include <asm/io.h>

struct hisi_device {
    struct power_device power;
    void *reset;
};

#define power_to_hisi(pdev) \
    container_of(pdev, struct hisi_device, power)

static void hisi_power_reset(struct power_device *pdev)
{
    struct hisi_device *hisi = power_to_hisi(pdev);
    writel(hisi->reset, 0xdeadbeef);
    mdelay(100);
}

static struct power_ops hisi_power_ops = {
    .reset = hisi_power_reset,
};

static state hisi_power_probe(struct platform_device *pdev, const void *pdata)
{
    struct hisi_device *hisi;
    uint32_t offset;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -ENODEV;

    if (dt_attribute_read_u32(pdev->dt_node, "reboot-offset", &offset))
        return -EINVAL;

    hisi = kzalloc(sizeof(*hisi), GFP_KERNEL);
    if (!hisi)
        return -ENOMEM;

    hisi->reset = (void *)platform_resource_start(pdev, 0) + offset;
    hisi->power.ops = &hisi_power_ops;
    hisi->power.dev = &pdev->dev;
    return power_register(&hisi->power);
}

static const struct dt_device_id hisi_power_ids[] = {
    { .compatible = "hisilicon,power" },
    { }, /* NULL */
};

static struct platform_driver hisi_power_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = hisi_power_ids,
    .probe = hisi_power_probe,
};

static state hisi_power_init(void)
{
    return platform_driver_register(&hisi_power_driver);
}
driver_initcall(hisi_power_init);