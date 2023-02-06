/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "power"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <power.h>
#include <pevent.h>
#include <export.h>
#include <printk.h>
#include <driver/power.h>
#include <asm/proc.h>

static const char *restart_name[] = {
    [POWER_RESTART_UNINIT] = "uninit",
    [POWER_RESTART_COLD] = "cold",
    [POWER_RESTART_WARM] = "warm",
    [POWER_RESTART_SOFT] = "soft",
    [POWER_RESTART_HARD] = "hard",
};

void __weak __noreturn proc_halt(void)
{
    irq_local_disable();
    for (;;);
}
EXPORT_SYMBOL(proc_halt);

void __weak __noreturn proc_reset(void)
{
    proc_halt();
}
EXPORT_SYMBOL(proc_reset);

void __weak __noreturn proc_poweroff(void)
{
    proc_halt();
}
EXPORT_SYMBOL(proc_poweroff);

/**
 * kernel_restart - restart the system
 * NOTE: Handle all shutdown events and then restart.
 */
void kernel_restart(enum restart_mode mode)
{
    pevent_shutdown();
    pr_emerg("kernel restart: %s mode\n", restart_name[mode]);

    switch (mode) {
        case POWER_RESTART_COLD:
            power_reset();
            break;

        case POWER_RESTART_WARM:
            power_restart();
            break;

        case POWER_RESTART_SOFT:
            proc_reset();

        default:
            break;
    }

    kernel_halt();
    proc_halt();
}
EXPORT_SYMBOL(kernel_restart);

void kernel_reboot(enum restart_mode mode)
{
    pevent_shutdown();
    pr_emerg("kernel reboot\n");

    /* Try all restart methods */
    power_reset();
    power_restart();
    proc_reset();
    proc_halt();
}
EXPORT_SYMBOL(kernel_reboot);

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
