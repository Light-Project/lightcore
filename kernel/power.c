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

/**
 * kernel_restart - restart the system
 * NOTE: Handle all shutdown events and then restart.
 */
void kernel_restart(void)
{
    pevent_shutdown();
    pr_emerg("kernel restarting\n");
    power_restart();
    proc_reset();
}
EXPORT_SYMBOL(kernel_restart);

/**
 * kernel_halt - halt the system
 * NOTE: Handle all shutdown events and then halt.
 */
void kernel_halt(void)
{
    pevent_shutdown();
    pr_emerg("kernel halted\n");
    proc_halt();
}
EXPORT_SYMBOL(kernel_halt);

/**
 * kernel_poweroff - poweroff the system
 * NOTE: Handle all shutdown events and then shutdown.
 */
void kernel_poweroff(void)
{
    pevent_shutdown();
    pr_emerg("kernel poweroff\n");
    power_shutdown();
    proc_poweroff();
}
EXPORT_SYMBOL(kernel_poweroff);

