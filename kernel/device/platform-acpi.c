/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/platform.h>

const struct acpi_device_id *
platform_acpi_match(struct platform_driver *pdrv, struct platform_device *pdev)
{
    return NULL;
}
