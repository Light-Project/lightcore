/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <initcall.h>
#include <spinlock.h>
#include <kshell.h>
#include <selftest.h>
#include <printk.h>
#include <export.h>

static LIST_HEAD(selftest_list);
static SPIN_LOCK(selftest_lock);

static struct selftest_command *selftest_find(const char *name)
{
    struct selftest_command *cmd;
    char *separate;

    if (strcount(name, ':') != 1)
        return NULL;

    separate = strchr(name, ':');
    *separate++ = '\0';

    spin_lock(&selftest_lock);
    list_for_each_entry(cmd, &selftest_list, list) {
        if (strcmp(cmd->group, name))
            continue;
        if (strcmp(cmd->name, separate))
            continue;
        return cmd;
    }
    spin_unlock(&selftest_lock);

    return NULL;
}

state selftest_register(struct selftest_command *cmd)
{
    if (!cmd->testing || !cmd->group || !cmd->name)
        return -EINVAL;

    spin_lock(&selftest_lock);
    list_add_prev(&selftest_list, &cmd->list);
    spin_unlock(&selftest_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(selftest_register);

void selftest_unregister(struct selftest_command *cmd)
{
    spin_lock(&selftest_lock);
    list_del(&cmd->list);
    spin_unlock(&selftest_lock);
}
EXPORT_SYMBOL(selftest_unregister);

static void usage(void)
{
    struct selftest_command *cmd;

    spin_lock(&selftest_lock);
    list_for_each_entry(cmd, &selftest_list, list) {
        printk("  %s:%s\t- %s\n", cmd->group, cmd->name, cmd->desc);
    }
    spin_unlock(&selftest_lock);
}

static state selftest_one(struct selftest_command *cmd, unsigned int count, int argc, char *argv[])
{
    void *pdata = NULL;
    state ret;

    if (cmd->prepare) {
        pdata = cmd->prepare(argc, argv);
        if ((ret = PTR_ERR(pdata)))
            return ret;
    }

    while (count--) {
        ret = cmd->testing(pdata);
        if (ret)
            break;
    }

    if (cmd->release)
        cmd->release(pdata);

    return ret;
}

static void selftest_all(unsigned int count)
{
    struct selftest_command *cmd;

    spin_lock(&selftest_lock);
    list_for_each_entry(cmd, &selftest_list, list)
        selftest_one(cmd, count, 0, NULL);
    spin_unlock(&selftest_lock);
}

static state selftest_main(int argc, char *argv[])
{
    struct selftest_command *cmd;
    unsigned int count, loop = 1;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'c':
                if ((++count) >= argc)
                    goto usage;
                loop = atoi(argv[count]);
                if (!loop)
                    goto usage;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (count == argc)
        selftest_all(loop);

    else if (count < argc) {
        cmd = selftest_find(argv[count]);
        if (!cmd)
            goto usage;
        selftest_one(cmd, loop, argc - count + 1, &argv[count]);
    }

    else
        goto usage;

    return -ENOERR;

usage:
    usage();
    return -EINVAL;
}

static struct kshell_command clear_cmd = {
    .name = "selftest",
    .desc = "kernel modules selftest",
    .exec = selftest_main,
};

static state clear_init(void)
{
    return kshell_register(&clear_cmd);
}
kshell_initcall(clear_init);
