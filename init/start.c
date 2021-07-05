/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <mm.h>
#include <string.h>
#include <src.h>
#include <asm-generic/header.h>
#include <system.h> 
#include <printk.h>
#include <console.h>

#include <driver/dt.h>
#include <driver/acpi.h>    

#include <init/init.h>
#include <init/initcall.h>  /* For initcalls */
#include <asm/irq.h>        /* For irq */
#include <driver/clocksource.h>
#include "private.h"        /* Private fun */

char boot_args[boot_args_size];

static void device_framework_init()
{
#ifdef CONFIG_DT
    if(dtb_start != NULL)
        early_dt_scan(dtb_start);
#endif
#ifdef CONFIG_ACPI
    acpi_table_init();
#endif
}

static void __init command_setup(const char *str)
{
}

static void __init mem_init(void)
{
    // arch_mem_init();

    page_init();
    
    /* Initialize kernel heap */
    kmem_init();
    
}

void __init kernel_start(const char *direct_args)
{
    /* entry criticality */
    arch_irq_disable();

    /* From this point, pre-printk is ready */
    arch_setup(boot_args);

    arch_irq_enable();

    device_framework_init();
    command_setup(direct_args);
    
    src_terminal_logo();

    mem_init();

    // sched_init();

    /* Start system timer */
//     timer_init();

    /* From this point, printk is ready */
    console_init();

    /* From here on, all kernel drivers are ready */
    initcalls();

    init_task();
}
