/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <console.h>
#include <initcall.h>
#include <printk.h>

/* the console list head */
static LIST_HEAD(console_list);
static struct console *current_console;

void console_write(const char *str, unsigned int len)
{
    struct console *console;

    list_for_each_entry(console, &console_list, list) {
        if (!(console->flags & CONSOLE_ENABLED))
            continue;
        if(!console->ops->write)
            continue;
        return console->ops->write(console, str, len);
    }

    pre_console_write(str, len);
}

void console_startup(struct console *con)
{
    if (con->flags & CONSOLE_ENABLED ||
        !con->ops->startup)
        return;

    con->flags |= CONSOLE_ENABLED;
}

void console_shutdown(struct console *con)
{
    if (~(con->flags & CONSOLE_ENABLED) ||
        !con->ops->shutdown)
        return;

    con->flags &= ~CONSOLE_ENABLED;
}

void console_register(struct console *con)
{
    struct console *tmp;

    list_for_each_entry(tmp, &console_list, list)
    if (tmp == con) {
        pr_warn("console %s already register\n",
                tmp->name);
        return;
    }

    if (!current_console) {
        console_startup(con);
        current_console = con;
    }

    list_add_prev(&console_list, &con->list);
}

void console_unregister(struct console *con)
{
    struct console *tmp;
    int val;

    list_for_each_entry(tmp, &console_list, list) {
        if (tmp == con)
            val = 1;
    } if (!val) {
        return;
    }

    console_shutdown(con);
    list_del(&con->list);
}

void __init console_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;
    int ret;

    for(fn = _ld_console_initcall_start;
        fn < _ld_console_initcall_end;
        fn++)
    {
        call = initcall_from_entry(fn);
        ret = call();
        if(ret)
            pr_warn("console: init error\n");
    }
}

static state console_bootarg(const char *arg)
{
    return -ENOERR;
}
bootarg_initcall("console", console_bootarg);
