/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <linkage.h>
#include <kernel.h> 
#include <kernel/sched.h>

#include <init/init.h>
#include <init/initcall.h>

#include <mm.h>
#include <memtest.h>

#include <logo.h>
#include <console.h>
#include <printk.h>

#include <device/driver.h>
#include <driver/irqchip.h>
#include <driver/clocksource.h>  

#include <asm-generic/header.h>
#include <asm/irq.h>

char boot_args[boot_args_size];

void init_task(void);

static void __init command_setup(void)
{
    const char *args = (void *)(size_t)boot_head.para[0];
    if(!args)
        return;
    strlcpy(boot_args, args, boot_args_size);
}

asmlinkage __visible void __init kernel_start(void)
{
    arch_irq_disable();
    arch_setup();
    terminal_logo();

    /* Initialize kernel parameters */
    command_setup();
    early_device_init();
    pr_notice("Kernel version: "CONFIG_VERSION"\n");
    pr_notice("Kernel command: %s\n", boot_args);

    mem_init();
    device_init();

    // sched_init();

    irqchip_init();
    timer_init();

    arch_irq_enable();

    console_init();
    
    initcalls();

    init_task();
}
