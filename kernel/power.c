/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "power: " fmt

#include <power.h>
#include <pevent.h>
#include <export.h>
#include <printk.h>
#include <driver/power.h>
#include <asm/proc.h>

void kernel_restart(void)
{
    pevent_shutdown();

    pr_emerg("kernel restarting\n");
    power_reset();
    power_restart();
    proc_reset();
}
EXPORT_SYMBOL(kernel_restart);

void kernel_halt(void)
{
    pevent_shutdown();

    pr_emerg("kernel halted\n");
    proc_halt();
}
EXPORT_SYMBOL(kernel_halt);

void kernel_poweroff(void)
{
    pevent_shutdown();

    pr_emerg("kernel poweroff\n");
    power_shutdown();
    proc_poweroff();
}
EXPORT_SYMBOL(kernel_poweroff);

