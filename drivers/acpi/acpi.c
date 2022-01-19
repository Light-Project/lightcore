/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "ACPI: " fmt

#include <driver/acpi.h>

state __init acpi_init(void)
{
    ACPI_STATUS status;

    pr_info("version: 0x%08x\n", ACPI_CA_VERSION);

    status = AcpiReallocateRootTable();
    if (ACPI_FAILURE(status)) {
        pr_err("unable to reallocate tables\n");
    }

    /* initialize acpi subsystem */
    status = AcpiInitializeSubsystem();
    if (ACPI_FAILURE(status)) {
        pr_err("unable to initialize the subsystem\n");
    }

    return -ENOERR;
}
