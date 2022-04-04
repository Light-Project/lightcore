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

static void usage(void)
{
    struct selftest_command *cmd;
    char buff[64];

    kshell_printf("usage: selftest [option] [group:name]...\n");
    kshell_printf("\t-c  <count>  number of repetitions for each test\n");
    kshell_printf("\t-i  output detailed debugging information\n");
    kshell_printf("\t-h  show this help\n");
    kshell_printf("Supported selftests:\n");

    spin_lock(&selftest_lock);
    list_for_each_entry(cmd, &selftest_list, list) {
        strncpy(buff, cmd->group, sizeof(buff));
        strncat(buff, ":", sizeof(buff));
        strncat(buff, cmd->name, sizeof(buff));
        kshell_printf("\t%-16s - %s\n", buff, cmd->desc);
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

static int __printf(2, 3) selftest_msg(bool info, const char *str, ...)
{
    va_list args;
    int len = 0;

    if (info) {
        va_start(args,str);
        len = kshell_vprintf(str, args);
        va_end(args);
    }

    return len;
}

static state selftest_one(bool info, struct selftest_command *cmd, unsigned int count, int argc, char *argv[])
{
    void *pdata = NULL;
    unsigned int tmp;
    state ret;

    selftest_msg(info, "================================\n");
    selftest_msg(info, "Selftest one {%s:%s}\n", cmd->group, cmd->name);

    if (cmd->prepare) {
        selftest_msg(info, "Preparing...\n");
        pdata = cmd->prepare(argc, argv);
        ret = PTR_ERR(pdata);
        selftest_msg(info, "Prepared: (%d) %s\n",
            ret, ret ? MESSAGE_FAIL : MESSAGE_PASS);
        if (ret)
            return ret;
    }

    for (tmp = 1; tmp <= count; ++tmp) {
        selftest_msg(info, "Testing loop%u...\n", tmp);
        ret = cmd->testing(pdata);
        selftest_msg(info, "Tested (%d) %s\n",
            ret, ret ? MESSAGE_FAIL : MESSAGE_PASS);
        if (ret || kshell_ctrlc())
            break;
    }

    if (cmd->release) {
        selftest_msg(info, "Releasing...\n");
        cmd->release(pdata);
        selftest_msg(info, "Released.\n");
    }

    return ret;
}

static state selftest_all(bool info, unsigned int count)
{
    struct selftest_command *cmd;
    state ret;

    spin_lock(&selftest_lock);

    list_for_each_entry(cmd, &selftest_list, list) {
        ret = selftest_one(info, cmd, count, 0, NULL);
        if (ret || kshell_ctrlc())
            break;
    }

    spin_unlock(&selftest_lock);
    return ret;
}

static state selftest_main(int argc, char *argv[])
{
    struct selftest_command *cmd = NULL;
    unsigned int count, loop = 1;
    state ret = -ENOERR;
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
        ret = selftest_all(iflag, loop);

    else for ((void)((cmd = selftest_iter(argv[count], cmd)) || ({goto usage; 1;}));
              cmd && !ret && !kshell_ctrlc(); cmd = selftest_iter(argv[count], cmd)) {
        ret = selftest_one(iflag, cmd, loop, argc - count + 1, &argv[count]);
    }

    return ret;

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
