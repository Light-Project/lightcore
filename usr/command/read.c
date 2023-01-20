/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <sched.h>
#include <ascii.h>
#include <timekeeping.h>
#include <initcall.h>

static const char read_lftbl[] = {
    ASCII_LF, ASCII_CR, 0,
};

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: read [option] var\n");
    kshell_puts(ctx, "\t-s  does not echo the user's input\n");
    kshell_puts(ctx, "\t-n  returns after reading the specified number of characters\n");
    kshell_puts(ctx, "\t-t  times out after the specified time in millisecond\n");
    kshell_puts(ctx, "\t-r  refresh timeout after input\n");
}

static state read_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, index, offset, nchar = 64;
    bool refresh = false, silent = false;
    ktime_t start, timeout = 0;
    char *buffer;
    state retval;

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

            case 'r':
                refresh = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (count == argc)
        goto usage;

    buffer = kmalloc(nchar + 1, GFP_KERNEL);
    if (!buffer)
        return -ENOERR;

    if (timeout)
        start = timekeeping_get_time();

    for (offset = 0; offset < nchar; offset += index) {
        unsigned int linefeed;

        index = kshell_read(ctx, buffer + offset, nchar - offset);
        if (index) {
            if (!silent)
                kshell_write(ctx, buffer + offset, index);

            if (memchr(buffer + offset, ASCII_ETX, index)) {
                ctx->canceled = true;
                return -ECANCELED;
            }

            buffer[offset + index] = '\0';
            linefeed = strcspn(buffer + offset, read_lftbl);

            if (linefeed != index) {
                offset += linefeed;
                break;
            }

            if (timeout && refresh)
                start = timekeeping_get_time();
        }

        if (timeout && ktime_after(timekeeping_get_time(),
            ktime_add(start, timeout))) {
            retval = ETIMEDOUT;
            goto finish;
        }

        sched_yield();
    }

    retval = kshell_setenv(ctx, argv[count], buffer, true);

finish:
    if (!silent)
        kshell_putc(ctx, '\n');

    kfree(buffer);
    return retval;

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
