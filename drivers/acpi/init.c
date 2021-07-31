/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  drivers/acpi/init.c
 *
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "acpi: " fmt

#include <driver/acpi.h>

#define ACPI_TABLE_MAX  128

static struct acpi_table_desc acpi_table[ACPI_TABLE_MAX];

state __init acpi_table_init(void)
{
    ACPI_STATUS ret;


    /* initialize acpi interpreter */
    ret = AcpiInitializeSubsystem();
    if(ACPI_FAILURE(ret)) {
        pr_err("unable to initialize the ACPI interpreter\n");
    }

    ret = AcpiInitializeTables(acpi_table, ACPI_TABLE_MAX, 0);
    if(ACPI_FAILURE(ret))
        return -EINVAL;

    return -ENOERR;
}
