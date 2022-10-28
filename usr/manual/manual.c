/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <manual.h>
#include <kshell.h>
#include <string.h>
#include <spinlock.h>
#include <export.h>

static LIST_HEAD(manual_list);
static SPIN_LOCK(manual_lock);

static void usage(struct kshell_context *ctx)
{
    struct kshell_manual *manual;

    kshell_printf(ctx, "usage: man [option] cmdname\n");
    kshell_printf(ctx, "\t-h  display this message\n");
    kshell_printf(ctx, "Loaded manuals:\n");

    spin_lock(&manual_lock);
    list_for_each_entry(manual, &manual_list, list)
        kshell_printf(ctx, "\t%s\n", manual->name);
    spin_unlock(&manual_lock);
}

static struct kshell_manual *manual_find(const char *name)
{
    struct kshell_manual *manual, *find = NULL;

    spin_lock(&manual_lock);
    list_for_each_entry(manual, &manual_list, list) {
        if (strcmp(manual->name, name))
            continue;
        find = manual;
        break;
    }
    spin_unlock(&manual_lock);

    return find;
}

state kshell_manual_register(struct kshell_manual *manual)
{
    if (!manual->name || !manual->describe)
        return -EINVAL;

    if (manual_find(manual->name))
        return -EALREADY;

    spin_lock(&manual_lock);
    list_add(&manual_list, &manual->list);
    spin_unlock(&manual_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(kshell_manual_register);

void kshell_manual_unregister(struct kshell_manual *manual)
{
    spin_lock(&manual_lock);
    list_del(&manual->list);
    spin_unlock(&manual_lock);
}
EXPORT_SYMBOL(kshell_manual_unregister);

static state manual_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct kshell_manual *manual;
    unsigned int count;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'h':
                goto usage;
        }
    }

    if (argc != count + 1)
        goto usage;

    manual = manual_find(argv[count]);
    if (!manual)
        goto usage;

    kshell_write(ctx, "\e[2J\e[1;1H", 10);
    kshell_write(ctx, manual->describe, manual->length);

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command manual_cmd = {
    .name = "man",
    .desc = "kshell command manual",
    .exec = manual_main,
};

static state clear_init(void)
{
    return kshell_register(&manual_cmd);
}
kshell_initcall(clear_init);
