/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/acpi.h>

#define ACPI_MAX_TABLES 128
static struct acpi_table_desc initial_tables[ACPI_MAX_TABLES];

state early_acpi_scan(void)
{
    ACPI_STATUS ret;

    ret = AcpiInitializeTables(initial_tables, ACPI_MAX_TABLES, 0);
    if (ACPI_FAILURE(ret)) {
        pr_err("unable to initialize the interpreter\n");
        return -EINVAL;
    }

    return -ENOERR;
}
