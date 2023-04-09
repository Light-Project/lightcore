/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "acpi"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

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
