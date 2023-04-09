/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <panic.h>
#include <initcall.h>
#include <proc.h>
#include <delay.h>
#include <power.h>
#include <printk.h>
#include <export.h>

int panic_timeout = CONFIG_PANIC_TIMEOUT;
unsigned int panic_timestep = CONFIG_PANIC_TIMESTEP;

EXPORT_SYMBOL(panic_timeout);
EXPORT_SYMBOL(panic_timestep);

DEFINE_NOTIFIER_SPIN_HEAD(panic_notifier);
DEFINE_NOTIFIER_SPIN_HEAD(panic_blink);

EXPORT_SYMBOL(panic_notifier);
EXPORT_SYMBOL(panic_blink);

static state panic_bootarg(char *args)
{
    char *options;

    /* get timeout and split commands */
    options = strchr(args, ':');
    if (options) {
        *options++ = '\0';
        panic_timeout = atoi(options);
    }

    panic_timestep = atoui(args);
    return -ENOERR;
}
bootarg_initcall("panic_timeout", panic_bootarg);

void __noreturn panic(const char *fmt, ...)
{
    unsigned int count, state;
    char buff[512];
    va_list args;

    irq_local_disable();
    preempt_disable();

    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    va_end(args);

    pr_emerg("Kernel Panic: %s\n", buff);
    notifier_spin_chain_call(&panic_notifier, NULL, -1, NULL);

    pr_emerg("---[ end Kernel panic ]---\n");
    irq_local_enable();

    if (panic_timeout < 0)
        kernel_reboot();

    if (panic_timeout > 0)
        pr_emerg("Rebooting in %d millisecond...\n", panic_timeout);

    for (count = state = 0; !panic_timeout ||
         count < panic_timeout; count += panic_timestep) {
        notifier_spin_chain_call(&panic_blink, (void *)(state ^= 1), -1, NULL);
        mdelay(panic_timestep);
    }

    kernel_reboot();
}
EXPORT_SYMBOL(panic);
