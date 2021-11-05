/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "start: " fmt

#include <string.h>
#include <linkage.h>
#include <kernel.h>
#include <sched.h>

#include <init.h>
#include <initcall.h>

#include <mm.h>
#include <memtest.h>

#include <fs.h>

#include <logo.h>
#include <console.h>
#include <printk.h>

#include <device/driver.h>
#include <driver/clk.h>
#include <driver/irqchip.h>
#include <driver/clocksource.h>

#include <asm-generic/header.h>
#include <asm/proc.h>
#include <asm/irq.h>

struct task init_task = {

};

void __noreturn user_init(void);

static void __init command_setup(void)
{
    const char *args = (void *)(size_t)boot_head.para;
    if(!args)
        return;
    strlcpy(boot_args, args, boot_args_size);
}

static void __init mount_rootfs(void)
{

}

asmlinkage __visible __init __noreturn void kernel_start(void)
{
    cpu_irq_disable();

    /* early init */
    pre_printk_init();
    terminal_logo();
    early_device_init();
    command_setup();

    /* arch init */
    arch_setup();

    pr_info("Kernel version: "CONFIG_VERSION"\n");
    pr_info("Kernel command: %s\n", boot_args);

    /* kernel init */
    mem_init();
    sched_init();

    /* driver init */
    device_init();
    clk_init();
    irqchip_init();
    timer_init();
    cpu_irq_enable();

    console_init();
    fork_init();

    /* late init */
    vfl_init();
    initcalls();

    /* init task */
    mount_rootfs();
    user_init();
}
