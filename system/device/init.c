/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <device.h>
#include <driver/dt.h>
#include <driver/acpi.h>
#include <driver/platform.h>

#include <asm-generic/header.h>

void __init early_dt_init(void)
{
    void *dt_start = (void *)(size_t)boot_head.para[1];

#ifdef CONFIG_BUILTIN_DTB
    if(!dt_start)
        dt_start = &_ld_init_dtb_start;
#endif

    early_dt_scan(dt_start);
}

void __init early_device_init(void)
{
    /* Initialize device core */
    bus_init();
    platform_bus_init();

#ifdef CONFIG_DT
    early_dt_init();
#endif
#ifdef CONFIG_ACPI
    acpi_table_init();
#endif
}