/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <device.h>
#include <driver/dt/fdt.h>
#include <driver/acpi.h>
#include <driver/platform.h>
#include <driver/firmware/dmi.h>
#include <asm-generic/header.h>

void __init early_device_init(void)
{
#ifdef CONFIG_DT
    void *dt_start;
    dt_start = (void *)(size_t)boot_head.dtb;
#ifdef CONFIG_BUILTIN_DTB
    if(!dt_start)
        dt_start = _ld_init_dtb_start;
#endif
    early_dt_scan(dt_start);
#endif /* CONFIG_DT */
}

void __init device_init(void)
{
    /* Initialize device core */
    bus_init();

#ifdef CONFIG_DT
    dt_init();
#endif

#ifdef CONFIG_ACPI
    early_acpi_scan();
    acpi_init();
#endif

#ifdef CONFIG_FIRMWARE_DMI
    dmi_init();
#endif

    platform_bus_init();
    devres_init();
}
