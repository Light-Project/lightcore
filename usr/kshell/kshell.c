/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <ascii.h>
#include <setjmp.h>
#include <kmalloc.h>
#include <initcall.h>
#include <console.h>
#include <export.h>

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
    state retval;
    char **argv;
    int argc;

    while (cmdline) {
        retval = kshell_parser(cmdline, (const char **)&cmdline, &argc, &argv);
        if (retval)
            return retval;

        if (buff && !strcmp(argv[0], "exit"))
            longjmp(buff, true);

        cmd = kshell_find(argv[0]);
        if (!cmd) {
            kshell_printf("kshell: command not found: %s\n", argv[0]);
            kfree(argv);
            return -EBADF;
        }

        retval = kshell_exec(cmd, argc, argv);
        kfree(argv);
        if (retval)
            return retval;
    }

    return -ENOERR;
}

state kshell_system(const char *cmdline)
{
    char *new;
    state ret;

    if (!cmdline) {
        kshell_printf("command inval\n");
        return -EINVAL;
    }

    new = strdup(cmdline);
    ret = do_system(new, NULL);
    kfree(new);

    return ret;
}
EXPORT_SYMBOL(kshell_system);

bool kshell_ctrlc(void)
{
    unsigned int len;
    char buff[32];

    len = console_read(buff, sizeof(buff));
    while (len--) {
        if (buff[len] == ASCII_ETX)
            return true;
    }

    return false;
}
EXPORT_SYMBOL(kshell_ctrlc);

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
        cmdline = readline(rstate, "kshell: /# ");
        if (!rstate->len)
            continue;
        ret = do_system(cmdline, &buff);
        snprintf(retbuf, sizeof(retbuf), "%d", ret);
        kshell_setenv("?", retbuf, true);
    }

    readline_free(rstate);
    return ret;
}

static state env_main(int argc, char *argv[])
{
    struct kshell_env *env;
    spin_lock(&kshell_env_lock);
    list_for_each_entry(env, &kshell_env_list, list)
        kshell_printf("\t%s=%s\n", env->name, env->val);
    spin_unlock(&kshell_env_lock);
    return -ENOERR;
}

static state help_main(int argc, char *argv[])
{
    struct kshell_command *cmd;
    spin_lock(&kshell_lock);
    list_for_each_entry(cmd, &kshell_list, list)
        kshell_printf("\t%-16s - %s\n", cmd->name, cmd->desc);
    spin_unlock(&kshell_lock);
    return -ENOERR;
}

static struct kshell_command kshell_cmd = {
    .name = "kshell",
    .desc = "kshell interpreter",
    .exec = kshell_main,
};

static struct kshell_command env_cmd = {
    .name = "env",
    .desc = "displays all environment variable",
    .exec = env_main,
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
    return kshell_register(&kshell_cmd)
        || kshell_register(&env_cmd)
        || kshell_register(&help_cmd)
        || kshell_register(&exit_cmd);
}
kshell_initcall(kshell_init);
