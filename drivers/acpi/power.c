/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/acpi.h>
#include <driver/power.h>

struct power_device acpi_power_device;

static void acpi_shutdown(struct power_device *pdev)
{
    cpu_irq_disable();
    AcpiEnterSleepState(ACPI_STATE_S5);
}

static struct power_ops acpi_power_ops = {
    .shutdown = acpi_shutdown,
};

state __init acpi_power_init(void)
{
    acpi_power_device.ops = &acpi_power_ops;
    return power_register(&acpi_power_device);
}
driver_initcall(acpi_power_init);
