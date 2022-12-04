/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <initcall.h>
#include <kmalloc.h>
#include <crypto/base32.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: base32 [option] ...\n");
    kshell_printf(ctx, "\t-d  decode data\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state base32_main(struct kshell_context *ctx, int argc, char *argv[])
{
    bool decode = false;
    unsigned int count;
    size_t slen, blen;
    char *buff;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'd':
                decode = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc != count + 1)
        goto usage;

    slen = strlen(argv[count]);
    if (decode)
        blen = base32_decode_length(slen);
    else
        blen = base32_encode_length(slen);

    buff = kmalloc(blen, GFP_KERNEL);
    if (!buff)
        return -ENOMEM;

    if (decode)
        base32_decode(buff, argv[count], slen);
    else
        base32_encode(buff, argv[count], slen);

    kshell_printf(ctx, "%.*s\n", (int)blen, buff);
    kfree(buff);

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command base32_cmd = {
    .name = "base32",
    .desc = "base32 encode or decode",
    .exec = base32_main,
};

static state base32_init(void)
{
    return kshell_register(&base32_cmd);
}
kshell_initcall(base32_init);
