/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <kernel.h>
#include <initcall.h>
#include <memdump.h>
#include <uuid.h>
#include <delay.h>
#include <xmodem.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: xmodem [option]\n");
    kshell_puts(ctx, "\t-r  <times> transmission retry time\n");
    kshell_puts(ctx, "\t-t  <10ms> transmission timeout\n");
    kshell_puts(ctx, "\t-e  use extend packet size\n");
    kshell_puts(ctx, "\t-c  use crc16 as verification func\n");
    kshell_puts(ctx, "\t-y  receive test demo\n");
    kshell_puts(ctx, "\t-u  transmit test demo\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state xmodem_read(void *buff, int size, void *pdata)
{
    struct kshell_context *ctx = pdata;
    unsigned int timeout = 0;
    int read;

    while (timeout++ < 5) {
        read = kshell_read(ctx, buff, size);
        if (read) {
            buff += read;
            size -= read;
            if (!size)
                return -ENOERR;
            timeout = 0;
        }
        msleep(1);
    }

    return -ETIMEDOUT;
}

static state xmodem_write(const void *buff, int size, void *pdata)
{
    struct kshell_context *ctx = pdata;
    kshell_write(ctx, buff, size);
    return -ENOERR;
}

static int xmodem_dump(void *pdata, const char *fmt, ...)
{
    struct kshell_context *ctx = pdata;
    va_list args;
    int len;

    va_start(args, fmt);
    len = kshell_vprintf(ctx, fmt, args);
    va_end(args);

    return len;
}

static state receive_test(struct kshell_context *ctx, struct xmodem_ops *xops)
{
    uint8_t buff[128];
    int retval;

    kshell_printf(ctx, "After the transmission, "
                  "please press enter to view the result.\n");
    retval = xmodem_receive(xops, buff, sizeof(buff));
    if (retval < 0)
        return retval;

    kshell_scanf(ctx, " ");
    memdump_print(xmodem_dump, ctx, "", MEMDUMP_PREFIX_OFFSET,
                  buff, sizeof(buff), 16, 4, 16, true);

    return -ENOERR;
}

static state transmit_test(struct kshell_context *ctx, struct xmodem_ops *xops)
{
    char buff[2][UUID_STRING_LEN + 1];

    uuid_string_generate(buff[0]);
    uuid_string_generate(buff[1]);
    buff[0][UUID_STRING_LEN] = '\n';
    buff[1][UUID_STRING_LEN] = '\0';

    return xmodem_transmit(xops, buff, sizeof(buff));
}

static state xmodem_main(struct kshell_context *ctx, int argc, char *argv[])
{
    bool rtest = false, ttest = false;
    unsigned int retry = 6, timeout = 1000;
    unsigned int count;
    state retval;

    struct xmodem_ops xops = {
        .read = xmodem_read,
        .write = xmodem_write,
        .pdata = ctx,
    };

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'r':
                if ((++count) >= argc)
                    goto usage;
                retry = atoui(argv[count]);
                break;

            case 't':
                if ((++count) >= argc)
                    goto usage;
                timeout = atoui(argv[count]);
                break;

            case 'e':
                xops.extend = true;
                break;

            case 'c':
                xops.crc16 = true;
                break;

            case 'y':
                rtest = true;
                break;

            case 'u':
                ttest = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc != count)
        goto usage;

    xops.retry = retry;
    xops.timeout = timeout;

    if (rtest) {
        retval = receive_test(ctx, &xops);
        return retval;
    }

    if (ttest) {
        retval = transmit_test(ctx, &xops);
        return retval;
    }

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command xmodem_cmd = {
    .name = "xmodem",
    .desc = "xmodem file transfer",
    .exec = xmodem_main,
};

static state xmodem_init(void)
{
    return kshell_register(&xmodem_cmd);
}
kshell_initcall(xmodem_init);
