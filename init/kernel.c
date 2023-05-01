/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <init.h>
#include <initcall.h>
#include <filesystem.h>
#include <power.h>
#include <kshell.h>
#include <printk.h>
#include <delay.h>

static __initdata const char *init_argv[] = {
    NULL, "init", NULL
};

static __initdata const char *init_envp[] = {
    "HOME=/", NULL
};

static __init bool init_run(const char *file)
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

static __init void basic_init(void)
{
    ctors_init();
    initcalls();
}

static __noreturn void auto_reboot(void)
{
    unsigned int count;

    pr_emerg("No init executable found, system will reset:\n");
    for (count = 5; count; --count) {
        pr_emerg("%d %.*s\n", count, count, "******");
        mdelay(1000);
    }

    kernel_reboot();
}

int __noreturn kernel_init(void *arg)
{
    state ret;

    basic_init();

    ret = mount_rootfs();
    if (ret) {
        init_run(CONFIG_DEFAULT_INIT);
        init_run("/bin/init");
        init_run("/sbin/init");
    }

    ksh_init();
    auto_reboot();
}
