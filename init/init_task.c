/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <stddef.h>
#include <fs.h>
#include <kernel.h>
#include <printk.h>

#define init_max_args   2
#define init_max_envs   3

static const char *init_argv[init_max_args] = {
    "init", NULL
};
static const char *init_envp[init_max_envs] = {
    "HOME=/", NULL
};

static bool init_run(const char *file)
{
    pr_info("Run %s as init process\n", file);
    return kernel_execve(file, init_argv, init_envp);
}

void init_task(void)
{
    bool ret;

    ret = init_run("/bin/init");
    if(ret)
        return;
    
    panic("No init executable found.");
}
 
