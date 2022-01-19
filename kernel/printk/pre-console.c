/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <console.h>
#include <initcall.h>
#include <printk.h>

static struct console *pre_console;

unsigned int pre_console_read(char *buff, unsigned int len)
{
    struct console_ops *ops = pre_console->ops;

    if (!pre_console || !ops->read)
        return 0;

    return ops->read(pre_console, buff, len);
}

void pre_console_write(const char *buff, unsigned int len)
{
    struct console_ops *ops = pre_console->ops;

    if (!pre_console && !ops->write)
        return;

    ops->write(pre_console, buff, len);
}

int pr_early(const char *fmt, ...)
{
    char buf[256];
    int len;
    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    pre_console_write(buf, len);
    return len;
}

void pre_console_register(struct console *con)
{
    if (!pre_console)
        pre_console = con;
}

void __weak pre_console_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, pconsole_initcall) {
        call = initcall_from_entry(fn);
        call();
    }
}
