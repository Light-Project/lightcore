/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <fs.h>
#include <driver/power.h>
#include <asm/delay.h>
#include <asm/proc.h>
#include <printk.h>

static const char *init_argv[] = {
    "init", NULL
};

static const char *init_envp[] = {
    "HOME=/", NULL
};

void __weak ksh_main(void) {}

static bool init_run(const char *file)
{
    pr_info("Run %s as init process\n", file);
    return kernel_execve(file, init_argv, init_envp);
}

void __noreturn init_task(void)
{
    unsigned int count;

    init_run("/bin/init");
    init_run("/sbin/init");
    init_run("/usr/bin/init");
    init_run("/usr/sbin/init");

    ksh_main();

    /* Startup failed reset the machine */
    pr_emerg("No init executable found, system will reset:");
    for (count = 5; count; count--) {
        pr_emerg(" %d", count);
        mdelay(1000);
    }
    pr_emerg("\n");

    power_reset();
    power_restart();
}
