/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "piix-power"

#include <initcall.h>
#include <driver/platform.h>
#include <driver/power.h>
#include <driver/power/piix.h>
#include <delay.h>
#include <asm/io.h>

struct piix_device {
    struct power_device power;
    resource_size_t base;
};

#define power_to_piix(pdev) \
    container_of(pdev, struct piix_device, power)

static void piix_power_restart(struct power_device *pdev)
{
    struct piix_device *piix = power_to_piix(pdev);

    /* CPU Soft Restart */
    outb(piix->base, PIIX_RC_RCPU | PIIX_RC_SRST);
    mdelay(100);
}

static void piix_power_reset(struct power_device *pdev)
{
    struct piix_device *piix = power_to_piix(pdev);

    /* Full Power Reset */
    outb(piix->base, PIIX_RC_FULL | PIIX_RC_RCPU | PIIX_RC_SRST);
    mdelay(100);
}

static struct power_ops piix_power_ops = {
    .restart = piix_power_restart,
    .reset = piix_power_reset,
};

static state piix_power_probe(struct platform_device *pdev, const void *pdata)
{
    struct piix_device *piix;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENODEV;

    piix = platform_kzalloc(pdev, sizeof(*piix), GFP_KERNEL);
    if (!piix)
        return -ENOMEM;

    piix->base = platform_resource_start(pdev, 0);
    piix->power.ops = &piix_power_ops;
    return power_register(&piix->power);
}

static const struct dt_device_id piix_power_ids[] = {
    { .compatible = "intel,piix-power" },
    { }, /* NULL */
};

static struct platform_driver piix_power_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = piix_power_ids,
    .probe = piix_power_probe,
};

static state piix_power_init(void)
{
    return platform_driver_register(&piix_power_driver);
}
driver_initcall(piix_power_init);
