/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "acpi_pm"

#include <initcall.h>
#include <irq.h>
#include <kmalloc.h>
#include <ioops.h>
#include <delay.h>
#include <driver/acpi.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/clocksource.h>

#define ACPI_PM_MONOTONICITY    10
#define ACPI_PM_READ_CHECKS     10000
#define PMTMR_TICK_FREQ         3579545

struct acpi_pm_device {
    struct clocksource_device clksrc;
    resource_size_t base;
};

#define clksrc_to_acpi(cdev) \
    container_of(cdev, struct acpi_pm_device, clksrc)

static uint32_t pmtmr_read(resource_size_t base)
{
    return inl(base) & BIT_RANGE(23, 0);
}

static uint64_t acpi_pm_read(struct clocksource_device *cdev)
{
    struct acpi_pm_device *adev = clksrc_to_acpi(cdev);
    return (uint64_t)pmtmr_read(adev->base);
}

static struct clocksource_ops acpi_pm_ops = {
    .read = acpi_pm_read,
};

static state acpi_pm_probe(struct platform_device *pdev, const void *pdata)
{
    struct acpi_pm_device *adev;

    adev = platform_kzalloc(pdev, sizeof(*adev), GFP_KERNEL);
    if (!adev)
        return -ENOMEM;

    adev->base = platform_resource_start(pdev, 0);
    adev->clksrc.flags = CLKSRC_CONTINUOUS;
    adev->clksrc.rating = DEVICE_RATING_GOOD;
    adev->clksrc.device = &pdev->dev;
    adev->clksrc.ops = &acpi_pm_ops;
    platform_set_devdata(pdev, adev);

    return clocksource_config_register(
        &adev->clksrc, PMTMR_TICK_FREQ,
        BIT_RANGE(23, 0)
    );
}

static struct platform_driver acpi_pm_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = acpi_pm_probe,
};

static state acpi_pm_detect(resource_size_t base)
{
    unsigned int count1, count2;
    uint32_t start, end;

    for (count1 = 0; count1 < ACPI_PM_MONOTONICITY; ++count1) {
        udelay(100 * count1);
        start = pmtmr_read(base);
        for (count2 = 0; count2 < ACPI_PM_READ_CHECKS; ++count2) {
            end = pmtmr_read(base);
            if (start == end)
                continue;
            if (start < end)
                break;
            if (start > end && end < 0xfff)
                break;
            return -ENODEV;
        }
        if (count2 == ACPI_PM_READ_CHECKS) {
            return -ENODEV;
        }
    }

    return -ENOERR;
}

static state acpi_pm_init(void)
{
    struct platform_device *pdev;
    struct resource *res;
    resource_size_t base;

    if (AcpiGbl_FADT.Header.Revision < 3)
        base = AcpiGbl_FADT.PmTimerBlock;
    else {
        if (AcpiGbl_FADT.XPmTimerBlock.SpaceId != ACPI_ADR_SPACE_SYSTEM_IO)
            return -ENODEV;
        base = AcpiGbl_FADT.XPmTimerBlock.Address;
        if (!base)
            base = AcpiGbl_FADT.PmTimerBlock;
    }

    if (!base || acpi_pm_detect(base))
        return -ENODEV;

    res = kzalloc(sizeof(*res), GFP_KERNEL);
    if (!res)
        return -ENOMEM;

    res->start = base;
    res->size = 1;

    pdev = platform_unified_register(&acpi_pm_driver, res, 1);
    if (!pdev)
        return -ENOMEM;

    return -ENOERR;
}
clockevent_initcall(acpi_pm_init);
