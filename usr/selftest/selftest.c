/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <initcall.h>
#include <spinlock.h>
#include <kshell.h>
#include <selftest.h>
#include <export.h>

#define MESSAGE_PASS  "[\e[32mPASS\e[0m]"
#define MESSAGE_FAIL  "[\e[31mFAIL\e[0m]"

static LIST_HEAD(selftest_list);
static SPIN_LOCK(selftest_lock);

static struct selftest_command *selftest_find(const char *group, const char *name)
{
    struct selftest_command *cmd, *find = NULL;

    spin_lock(&selftest_lock);
    list_for_each_entry(cmd, &selftest_list, list) {
        if (strcmp(cmd->group, group))
            continue;
        if (strcmp(cmd->name, name))
            continue;
        find = cmd;
        break;
    }
    spin_unlock(&selftest_lock);

    return find;
}

state selftest_register(struct selftest_command *cmd)
{
    if (!cmd->testing || !cmd->group || !cmd->name)
        return -EINVAL;

    if (selftest_find(cmd->group, cmd->name))
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

static void usage(struct kshell_context *ctx)
{
    struct selftest_command *cmd;
    char buff[64];

    kshell_printf(ctx, "usage: selftest [option] [group:name]...\n");
    kshell_printf(ctx, "\t-c  <count>  number of repetitions for each test\n");
    kshell_printf(ctx, "\t-i  output detailed debugging information\n");
    kshell_printf(ctx, "\t-h  show this help\n");
    kshell_printf(ctx, "Supported selftests:\n");

    spin_lock(&selftest_lock);
    list_for_each_entry(cmd, &selftest_list, list) {
        strncpy(buff, cmd->group, sizeof(buff));
        strncat(buff, ":", sizeof(buff));
        strncat(buff, cmd->name, sizeof(buff));
        kshell_printf(ctx, "\t%-16s - %s\n", buff, cmd->desc);
    }
    spin_unlock(&selftest_lock);
}

static struct selftest_command *selftest_iter(const char *name, struct selftest_command *prev)
{
    struct selftest_command *cmd, *find = NULL;
    char *separate;

    if ((separate = strchr(name, ':')))
        separate[0] = '\0';

    spin_lock(&selftest_lock);

    if (!prev)
        prev = container_of(&selftest_list, struct selftest_command, list);

    for (cmd = list_next_entry(prev, list);
         !list_check_head(&selftest_list, &cmd->list);
         cmd = list_next_entry(cmd, list)) {
        if (strcmp(cmd->group, name))
            continue;
        if (separate && strcmp(cmd->name, &separate[1]))
            continue;
        find = cmd;
        break;
    }

    spin_unlock(&selftest_lock);

    if (separate)
        separate[0] = ':';

    return (!prev && separate) ? NULL : find;
}

static int __printf(3, 4) selftest_msg(struct kshell_context *ctx, bool info, const char *str, ...)
{
    va_list args;
    int len = 0;

    if (info) {
        va_start(args,str);
        len = kshell_vprintf(ctx, str, args);
        va_end(args);
    }

    return len;
}

static state selftest_one(struct kshell_context *ctx, struct selftest_command *cmd,
                          bool info, unsigned int count, int argc, char *argv[])
{
    void *pdata = NULL;
    unsigned int tmp;
    state retval;

    selftest_msg(ctx, info, "================================\n");
    selftest_msg(ctx, info, "Selftest one {%s:%s}\n", cmd->group, cmd->name);

    if (cmd->prepare) {
        selftest_msg(ctx, info, "Preparing...\n");
        pdata = cmd->prepare(ctx, argc, argv);
        retval = PTR_ERR(pdata);
        selftest_msg(ctx, info, "Prepared: (%d) %s\n",
            retval, retval ? MESSAGE_FAIL : MESSAGE_PASS);
        if (retval)
            return retval;
    }

    for (tmp = 1; tmp <= count; ++tmp) {
        selftest_msg(ctx, info, "Testing loop%u...\n", tmp);
        retval = cmd->testing(ctx, pdata);
        selftest_msg(ctx, info, "Tested (%d) %s\n",
            retval, retval ? MESSAGE_FAIL : MESSAGE_PASS);
        if (kshell_ctrlc(ctx))
            retval = -ECANCELED;
        if (retval)
            break;
    }

    if (cmd->release) {
        selftest_msg(ctx, info, "Releasing...\n");
        cmd->release(ctx, pdata);
        selftest_msg(ctx, info, "Released.\n");
    }

    return retval;
}

static state selftest_all(struct kshell_context *ctx, bool info, unsigned int count)
{
    struct selftest_command *cmd;
    state retval;

    spin_lock(&selftest_lock);

    list_for_each_entry(cmd, &selftest_list, list) {
        retval = selftest_one(ctx, cmd, info, count, 0, NULL);
        if (retval)
            break;
    }

    spin_unlock(&selftest_lock);
    return retval;
}

static state selftest_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct selftest_command *cmd;
    unsigned int count, loop = 1;
    state retval = -ENOERR;
    bool iflag = false;

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

            case 'i':
                iflag = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (count == argc)
        retval = selftest_all(ctx, iflag, loop);

    else for ((void)((cmd = selftest_iter(argv[count], NULL)) || ({goto usage; 1;}));
              cmd && !retval && !kshell_ctrlc(ctx); cmd = selftest_iter(argv[count], cmd)) {
        retval = selftest_one(ctx, cmd, iflag, loop, argc - count + 1, &argv[count]);
    }

    return retval;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command selftest_cmd = {
    .name = "selftest",
    .desc = "kernel modules selftest",
    .exec = selftest_main,
};

static state selftest_init(void)
{
    return kshell_register(&selftest_cmd);
}
kshell_initcall(selftest_init);
