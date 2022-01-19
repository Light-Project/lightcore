/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "ksh"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include "kshell.h"
#include <string.h>
#include <setjmp.h>
#include <kmalloc.h>
#include <initcall.h>
#include <console.h>
#include <export.h>
#include <printk.h>

static unsigned int readline_read(char *str, unsigned int len, void *data)
{
    return console_read(str, len);
}

static void readline_write(const char *str, unsigned int len, void *data)
{
    console_write(str, len);
}

static state do_system(char *cmdline, jmp_buf *buff)
{
    struct kshell_command *cmd;
    char **argv;
    int argc;
    state ret;

    while (cmdline) {
        ret = kshell_parser(cmdline, &cmdline, &argc, &argv);
        if (ret)
            return ret;

        cmd = kshell_find(argv[0]);
        if (!cmd) {
            pr_err("command not found: %s\n", argv[0]);
            return -EBADF;
        }

        if (buff && !strcmp(argv[0], "exit"))
            longjmp(buff, true);

        if (!cmd->exec)
            return -ENXIO;

        ret = cmd->exec(argc, argv);
        if (ret)
            return ret;
    }

    return ret;
}

state kshell_system(const char *cmdline)
{
    char *new;
    state ret;

    new = strdup(cmdline);
    ret = do_system(new, NULL);
    kfree(new);

    return ret;
}
EXPORT_SYMBOL(kshell_system);

state kshell_main(int argc, char *argv[])
{
    struct readline_state *rstate;
    char *cmdline, retbuf[20];
    jmp_buf buff;
    state ret, exit;

    rstate = readline_alloc(readline_read, readline_write, NULL);
    if (!rstate)
        return -ENOMEM;

    exit = setjmp(&buff);

    while (!exit) {
        cmdline = readline(rstate, "kshell: # ");
        if (!rstate->len)
            continue;
        ret = do_system(cmdline, &buff);
        snprintf(retbuf, sizeof(retbuf), "%d", ret);
        kshell_setenv("?", retbuf, true);
    }

    readline_free(rstate);
    return ret;
}

state help_main(int argc, char *argv[])
{
    struct kshell_command *cmd;
    spin_lock(&kshell_lock);
    list_for_each_entry(cmd, &kshell_list, list)
        printk("%s\t- %s\n", cmd->name, cmd->desc);
    spin_unlock(&kshell_lock);
    return -ENOERR;
}

static struct kshell_command kshell_cmd = {
    .name = "kshell",
    .desc = "kshell interpreter",
    .exec = kshell_main,
};

static struct kshell_command help_cmd = {
    .name = "help",
    .desc = "displays all available instructions",
    .exec = help_main,
};

static struct kshell_command exit_cmd = {
    .name = "exit",
    .desc = "exit the kshell interpreter",
};

static state kshell_init(void)
{
    kshell_register(&kshell_cmd);
    kshell_register(&help_cmd);
    kshell_register(&exit_cmd);
    return -ENOERR;
}
kshell_initcall(kshell_init);
