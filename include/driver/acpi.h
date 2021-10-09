/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_ACPI_H_
#define _DRIVER_ACPI_H_

#include <state.h>
#include <driver/acpi/acpi.h>

state acpi_init(void);
state early_acpi_scan(void);

#endif /* _DRIVER_ACPI_H_ */
