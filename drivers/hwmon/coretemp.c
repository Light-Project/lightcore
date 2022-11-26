/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "coretemp"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/hwmon.h>
#include <asm/regs.h>
#include <printk.h>

enum coretemp_hwmon_sensor {
    CORETEMP_DIE = 0,
    CORETEMP_PKG = 1,
};

static const char *coretemp_hwmon_label[] = {
    [CORETEMP_DIE] = "core",
    [CORETEMP_PKG] = "package",
};

static struct hwmon_channel_info coretemp_core_info[] = {
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_THERMAL,
        [CORETEMP_DIE] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
    ),
    { }, /* NULL */
};

static struct hwmon_channel_info coretemp_pkg_info[] = {
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_THERMAL,
        [CORETEMP_DIE] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
        [CORETEMP_PKG] = HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA,
    ),
    { }, /* NULL */
};

static state coretemp_read(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                           unsigned int index, uint32_t att, ssize_t *val)
{
    unsigned int msr;
    uint64_t value;

    switch (att) {
        case HWMON_THERMAL_DATA:
            if (index == CORETEMP_PKG)
                msr = MSR_IA32_PACKAGE_THERM_STATUS;
            else
                msr = MSR_IA32_THERM_STATUS;
            value = msr_get(msr);
            value = 100 - ((lower_32_bits(value) >> 16) & 0x7f);
            *val = value * 1000;
            break;

        default:
            return -EINVAL;
    }

    return -ENOERR;
}

static state coretemp_string(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                             unsigned int index, uint32_t att, const char **str)
{
    switch (att) {
        case HWMON_THERMAL_LABEL:
            *str = coretemp_hwmon_label[index];
            break;

        default:
            return -EINVAL;
    }

    return -ENOERR;
}

static struct hwmon_ops coretemp_ops = {
    .read = coretemp_read,
    .string = coretemp_string,
};

static state coretemp_probe(struct platform_device *pdev, const void *pdata)
{
    struct hwmon_device *hdev;

    hdev = platform_kzalloc(pdev, sizeof(*hdev), GFP_KERNEL);
    if (!hdev)
        return -ENOMEM;

    if (!cpuid_support(X86_CPUID_PTS))
        hdev->info = coretemp_core_info;
    else {
        pr_debug("support package thermal\n");
        hdev->info = coretemp_pkg_info;
    }

    hdev->dev = &pdev->dev;
    hdev->ops = &coretemp_ops;
    platform_set_devdata(pdev, hdev);

    return hwmon_register(hdev);
}

static struct platform_driver coretemp_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = coretemp_probe,
};

static state coretemp_init(void)
{
    struct platform_device *pdev;

    if (!cpuid_support(X86_CPUID_DTHERM))
        return -ENODEV;

    pdev = platform_unified_register(&coretemp_driver, NULL, 0);
    if (!pdev)
        return -ENOMEM;

    return -ENOERR;
}
driver_initcall(coretemp_init);
