/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "console"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <ctype.h>
#include <string.h>
#include <console.h>
#include <initcall.h>
#include <spinlock.h>
#include <printk.h>
#include <export.h>

struct console_preferred {
    char name[16];
    unsigned int index;
    const char *options;
    bool user;
};

#define CONSOLE_PREFERREDS CONFIG_CONSOLE_PREFERREDS
static struct console_preferred console_cmdline[CONSOLE_PREFERREDS];
static __read_mostly unsigned int console_preferred = -1;
static __read_mostly struct console *console_active;

static LIST_HEAD(console_list);
static SPIN_LOCK(console_lock);

unsigned int console_read(char *buff, unsigned int len)
{
    struct console_ops *ops;

    if (unlikely(!console_active))
        return earlycon_read(buff, len);

    ops = console_active->ops;
    if (!ops->read)
        return 0;

    return ops->read(console_active, buff, len);
}
EXPORT_SYMBOL(console_read);

void console_write(const char *buff, unsigned int len)
{
    struct console_ops *ops;

    if (unlikely(!console_active))
        return earlycon_write(buff, len);

    ops = console_active->ops;
    if (!ops->write)
        return;

    return ops->write(console_active, buff, len);
}
EXPORT_SYMBOL(console_write);

void console_sync(void)
{
    struct console_ops *ops;

    if (unlikely(!console_active))
        return earlycon_sync();

    ops = console_active->ops;
    if (!ops->sync)
        return;

    return ops->sync(console_active);
}
EXPORT_SYMBOL(console_sync);

static inline state console_startup(struct console *con)
{
    state retval;

    if (!con->ops->startup || console_test_enabled(con))
        return -ENOERR;

    if ((retval = con->ops->startup(con)))
        return retval;

    console_set_enabled(con);
    return -ENOERR;
}

static inline void console_shutdown(struct console *con)
{
    if (!con->ops->shutdown || !console_test_enabled(con))
        return;

    console_clr_enabled(con);
    con->ops->shutdown(con);
}

static struct console *console_find(const char *name, unsigned int index)
{
    struct console *walk, *find = NULL;

    list_for_each_entry(walk, &console_list, list) {
        if (walk->index == index && !strcmp(walk->name, name)) {
            find = walk;
            break;
        }
    }

    return find;
}

static state console_select(struct console *con, bool user)
{
    unsigned int count;

    for (count = 0; count < CONSOLE_PREFERREDS; ++count) {
        struct console_preferred *preferred = console_cmdline + count;

        /* end of the array */
        if (!*preferred->name)
            break;

        if (preferred->user != user)
            continue;

        if (strcmp(preferred->name, con->name))
            continue;

        console_set_enabled(con);
        if (count == console_preferred)
            console_set_enabled(con);

        return -ENOERR;
    }

    return -ENOENT;
}

void console_register(struct console *con)
{
    struct console *walk, *tmp;
    bool bootcon = false, realcon = false;
    state retval;

    if (!con->ops || !con->ops->write)
        return;

    spin_lock(&console_lock);
    if (console_find(con->name, con->index)) {
        spin_unlock(&console_lock);
        pr_warn("console %s%u already registered\n", con->name, con->index);
        return;
    }

    list_for_each_entry(walk, &console_list, list) {
        if (console_test_boot(walk))
            bootcon = true;
        else
            realcon = true;
    }

    if (realcon && console_test_boot(con)) {
        spin_unlock(&console_lock);
        pr_warn("boot console %s%u register to late\n", con->name, con->index);
        return;
    }

    if (!(retval = console_select(con, true)))
        retval = console_startup(con);
    else if (!(retval = console_select(con, false)))
        retval = console_startup(con);

    if (retval) {
        spin_unlock(&console_lock);
        return;
    }

    if (console_test_consdev(con) || !console_active) {
        if (console_active)
            console_clr_consdev(console_active);
        console_set_consdev(con);
        console_active = con;
    }

    if (bootcon && console_test_consdev(con) && !console_test_boot(con)) {
        list_for_each_entry_safe(walk, tmp, &console_list, list) {
            if (console_test_boot(con)) {
                console_shutdown(con);
                list_del(&walk->list);
            }
        }
    }

    list_add(&console_list, &con->list);
    spin_unlock(&console_lock);
    pr_info("register device %s%u\n", con->name, con->index);
}
EXPORT_SYMBOL(console_register);

void console_unregister(struct console *con)
{
    struct console *next;

    spin_lock(&console_lock);
    if (!console_find(con->name, con->index)) {
        spin_unlock(&console_lock);
        return;
    }

    list_del(&con->list);
    if (console_active == con) {
        next = list_first_entry_or_null(&console_list, struct console, list);
        console_active = next;
    }

    console_shutdown(con);
    spin_unlock(&console_lock);
    pr_info("unregister device %s%u\n", con->name, con->index);
}
EXPORT_SYMBOL(console_unregister);

state console_preferred_add(const char *name, unsigned int index, const char *options, bool user)
{
    struct console_preferred *preferred;
    unsigned int count;

    for (count = 0; count < CONSOLE_PREFERREDS && *console_cmdline[count].name; ++count) {
        if (!strcmp(console_cmdline[count].name, name) ||
            console_cmdline[count].index == index)
            return -EALREADY;
    }

    if (count == CONSOLE_PREFERREDS)
        return -ENOMEM;

    preferred = console_cmdline + count;
    preferred->user = user;
    preferred->index = index;
    preferred->options = options;

    console_preferred = count;
    strncpy(preferred->name, name, sizeof(preferred->name));

    return -ENOERR;
}

static state console_bootarg(char *args)
{
    char buff[sizeof(console_cmdline[0].name)];
    char *options, *indexstr;
    unsigned int index;

    if (!*args || !strcmp(args, "null")) {
        console_preferred_add("ttynull", 0, NULL, true);
        return -ENOERR;
    }

    /* get options and split commands */
    options = strchr(args, ',');
    if (options)
        *options++ = '\0';

    /* copy device name and index */
    if (isdigit(*args)) {
        strcpy(buff, "ttyS");
        strncpy(buff, args, sizeof(buff) - 5);
    } else {
        strncpy(buff, args, sizeof(buff) - 1);
    }
    buff[sizeof(buff) - 1] = '\0';

    /* get index and split buffer */
    for (indexstr = buff; *indexstr; ++indexstr) {
        if (isdigit(*indexstr) || *indexstr == ',')
            break;
    }

    index = atoui(indexstr);
    *indexstr = '\0';
    console_preferred_add(buff, index, options, true);

    return -ENOERR;
}
earlyarg_initcall("console", console_bootarg);

void __init console_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;
    int ret;

    initcall_for_each_fn(fn, console_initcall) {
        call = initcall_from_entry(fn);
        if ((ret = call()))
            pr_err("%s init failed, error code [%d]\n", fn->name, ret);
    }
}
