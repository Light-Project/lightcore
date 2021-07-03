/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <driver/acpi.h>

#define ACPI_TABLE_MAX  128

static struct acpi_table_desc acpi_table[ACPI_TABLE_MAX];

state __init acpi_table_init(void)
{
    ACPI_STATUS ret;

    ret = AcpiInitializeTables(acpi_table, ACPI_TABLE_MAX, 0);
    if(ACPI_FAILURE(ret))
        return -EINVAL;

    return -ENOERR;
}
