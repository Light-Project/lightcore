/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <console.h>
#include <initcall.h>
#include <printk.h>

static struct console *earlycon;

unsigned int earlycon_read(char *buff, unsigned int len)
{
    struct console_ops *ops;

    if (!earlycon)
        return 0;

    ops = earlycon->ops;
    if (!ops->read)
        return 0;

    return ops->read(earlycon, buff, len);
}

void earlycon_write(const char *buff, unsigned int len)
{
    struct console_ops *ops;

    if (!earlycon)
        return;

    ops = earlycon->ops;
    if (!ops->write)
        return;

    ops->write(earlycon, buff, len);
}

void earlycon_sync(void)
{
    struct console_ops *ops;

    if (!earlycon)
        return;

    ops = earlycon->ops;
    if (!ops->sync)
        return;

    ops->sync(earlycon);
}

int pr_early(const char *fmt, ...)
{
    char buf[256];
    int len;
    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    earlycon_write(buf, len);
    return len;
}

void earlycon_register(struct console *con)
{
    if (!earlycon)
        earlycon = con;
}

void __weak earlycon_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, earlycon_initcall) {
        call = initcall_from_entry(fn);
        call();
    }
}
