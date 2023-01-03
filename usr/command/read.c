/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <sched.h>
#include <ascii.h>
#include <timekeeping.h>
#include <initcall.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: read [option] var\n");
    kshell_puts(ctx, "\t-s  does not echo the user's input\n");
    kshell_puts(ctx, "\t-n  returns after reading the specified number of characters\n");
    kshell_puts(ctx, "\t-t  times out after the specified time in millisecond\n");
}

static state read_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, index, offset, nchar = 64;
    ktime_t start, timeout = 0;
    bool silent = false;
    char valbuf[65];

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 's':
                silent = true;
                break;

            case 'n':
                if ((++count) >= argc)
                    goto usage;
                nchar = atoui(argv[count]);
                break;

            case 't':
                if ((++count) >= argc)
                    goto usage;
                timeout = atoui(argv[count]);
                timeout = ms_to_ktime(timeout);
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (count == argc)
        goto usage;

    if (timeout)
        start = timekeeping_get_time();

    for (offset = 0; offset < nchar; offset += index) {
        char *lfptr, *crptr;

        index = kshell_read(ctx, valbuf + offset, nchar - offset);
        if (index) {
            if (!silent)
                kshell_write(ctx, valbuf + offset, index);

            if (memchr(valbuf + offset, ASCII_ETX, index)) {
                ctx->canceled = true;
                return -ECANCELED;
            }

            lfptr = memchr(valbuf + offset, ASCII_LF, index);
            crptr = memchr(valbuf + offset, ASCII_CR, index);

            if (lfptr || crptr) {
                if (crptr && crptr)
                    lfptr = min(lfptr, crptr);
                else
                    lfptr += (uintptr_t)crptr;
                offset = lfptr - valbuf;
                break;
            }
        }

        if (timeout && ktime_after(timekeeping_get_time(), ktime_add(start, timeout)))
            break;

        sched_yield();
    }

    if (!offset)
        return -ETIMEDOUT;

    if (!silent)
        kshell_putc(ctx, '\n');

    valbuf[offset] = '\0';
    return kshell_setenv(ctx, argv[count], valbuf, true);

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command reset_cmd = {
    .name = "read",
    .desc = "read terminal input",
    .exec = read_main,
};

static state reset_init(void)
{
    return kshell_register(&reset_cmd);
}
kshell_initcall(reset_init);
