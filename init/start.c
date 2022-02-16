/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "start: " fmt

#include <kernel.h>
#include <linkage.h>
#include <string.h>
#include <init.h>
#include <memory.h>
#include <task.h>
#include <kcoro.h>
#include <filesystem.h>

#include <logo.h>
#include <console.h>
#include <printk.h>
#include <panic.h>

#include <driver/clk.h>
#include <driver/irqchip.h>
#include <driver/clockevent.h>

#include <irqflags.h>
#include <asm-generic/header.h>

struct sched_task init_task = {
    .stack = init_stack,
};

static void __init command_setup(void)
{
    const char *args = (void *)(size_t)boot_head.cmd;
    if (!args)
        return;
    strlcpy(boot_args, args, boot_args_size);
}

static noinline __noreturn void kernel_main(void)
{
    user_init(NULL);
    fork_thread(0, user_init, NULL);
    idle_task_entry();
}

asmlinkage __visible __init __noreturn void kernel_start(void)
{
    task_stack_magic(&init_task);
    irq_local_disable();

    /* early arch */
    pre_console_init();
    terminal_logo();
    early_device_init();
    command_setup();
    arch_setup();

    pr_info("kernel version: "CONFIG_VERSION"\n");
    pr_info("kernel command: %s\n", boot_args);

    /* early kernel */
    mem_init();
    sched_init();
    kcoro_init();

    /* basic driver */
    device_init();
    irqchip_init();
    clk_init();
    clockevent_init();
    irq_local_enable();

    console_init();
    fork_init();

    /* late init */
    vfl_init();

    /* init task */
    kernel_main();
}
