/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "private.h"
#include <linkage.h>
#include <string.h>
#include <src.h>
#include <init/init.h>
#include <init/initcall.h>
#include <mm.h>
#include <memtest.h>
#include <device/driver.h>
#include <driver/clocksource.h>  
#include <system.h> 
#include <console.h>
#include <printk.h>

#include <asm-generic/header.h>
#include <asm/irq.h>

char boot_args[boot_args_size];

static void __init command_setup(void)
{
    const char *args = (void *)(size_t)boot_head.para[0];
    if(!args)
        return;
    strlcpy(boot_args, args, boot_args_size);
}

static void __init mem_init(void)
{
    mem_reserve(va_to_pa(&_ld_text_start),
            (size_t)&_ld_image_end - (size_t)&_ld_text_start);
    vmem_init();

    page_init();

    /* Initialize kernel heap */
    kmem_init();
}

asmlinkage __visible void __init kernel_start(void)
{
    arch_irq_disable();

    arch_setup(boot_args);
    src_terminal_logo();

    command_setup();
    mem_init();
    // sched_init();

    early_device_init();
    pre_printk("kernel command: %s\n\r", boot_args);

    timer_init();

    arch_irq_enable();

    console_init();

    initcalls();

    init_task();
}
