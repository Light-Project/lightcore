/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/platform.h>

const struct acpi_device_id *
platform_acpi_match(const struct acpi_device_id *table, const struct acpi_node *node)
{
    return NULL;
}
