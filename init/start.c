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
#include <idr.h>
#include <lru-cache.h>
#include <task.h>
#include <softirq.h>
#include <timer.h>
#include <pid.h>
#include <kcoro.h>
#include <kthread.h>
#include <stackprotector.h>
#include <namespace.h>
#include <filesystem.h>

#include <logo.h>
#include <console.h>
#include <printk.h>
#include <extable.h>
#include <panic.h>

#include <driver/clk.h>
#include <driver/irqchip.h>
#include <driver/clockevent.h>

#include <irqflags.h>
#include <asm-generic/header.h>

struct sched_task init_task = {
    .stack = init_stack,
    .namespace = &root_ns,
    .sched_type = &idle_sched,
    .flags = SCHED_TASK_KTHREAD,
};

static void __init command_setup(void)
{
    const char *args = (void *)(size_t)boot_head.cmd;
    if (!args)
        return;
    strlcpy(boot_args, args, BOOT_PARAM_SIZE);
}

static noinline __noreturn void kernel_main(void)
{
    kernel_clone(0, user_init, NULL);
    kernel_clone(0, kthread_daemon, NULL);
    idle_task_entry();
}

asmlinkage __visible __init __noreturn void kernel_start(void)
{
    task_stack_magic(&init_task);
    irq_local_disable();

    earlycon_init();
    pr_early(terminal_logo);

    early_device_init();
    command_setup();
    arch_setup();

    pr_info("kernel version: "CONFIG_VERSION"\n");
    pr_info("kernel command: %s\n", boot_args);
    earlyargs_init(boot_args);

    crash_init();
    extable_init();

    mem_init();
    idr_init();
    lrc_init();

    sched_init();
    boot_stack_canary();

    softirq_init();
    timer_init();
    bootargs_init(boot_args);

    device_init();
    irqchip_init();
    clk_init();
    clockevent_init();
    irq_local_enable();

    console_init();
    namespace_init();
    pid_init();

    clone_init();
    kcoro_init();

    vfl_init();
    kernel_main();
}
