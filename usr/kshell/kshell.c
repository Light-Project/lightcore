/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kshell.h>
#include <printk.h>

LIST_HEAD(kshell_list);

state kshell_register(struct kshell_command *cmd)
{
    if (!cmd->entry || !cmd->name)
        return -EINVAL;

    list_add(&kshell_list, &cmd->list);
    return -ENOERR;
}

void kshell_unregister(struct kshell_command *cmd)
{
    list_del(&cmd->list);
}

void __init ksh_initcall(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, kshell_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed", fn->name);
    }
}

void ksh_main(void)
{
    printk("########################\n");
    printk("Welcome to lightcore ksh\n");

    ksh_initcall();
    printk("Have a lot of fun..\n");
}
