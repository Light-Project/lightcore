/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <init.h>
#include <initcall.h>
#include <filesystem.h>
#include <power.h>
#include <kshell.h>
#include <printk.h>
#include <asm/delay.h>

static const char *init_argv[] = {
    NULL, "init", NULL
};

static const char *init_envp[] = {
    "HOME=/", NULL
};

static bool init_run(const char *file)
{
	const char *const *arg;

    init_argv[0] = file;
    pr_info("run %s as init task\n", file);
	pr_debug("  with arguments:\n");
	for (arg = init_argv; *arg; arg++)
		pr_debug("    %s\n", *arg);
	pr_debug("  with environment:\n");
	for (arg = init_envp; *arg; arg++)
		pr_debug("    %s\n", *arg);

    return kernel_execve(file, init_argv, init_envp);
}

static void __init mount_rootfs(void)
{

}

static void __init basic_init(void)
{
    initcalls();
}

int __noreturn user_init(void *arg)
{
    unsigned int count;

    basic_init();
    mount_rootfs();

    init_run(CONFIG_DEFAULT_INIT);
    init_run("/bin/init");
    init_run("/sbin/init");

    ksh_init();

    /* Startup failed reset the machine */
    pr_emerg("No init executable found, system will reset:");
    for (count = 5; count; count--) {
        pr_emerg(" %d", count);
        mdelay(1000);
    }
    pr_emerg("\n");

    kernel_reboot();
}
