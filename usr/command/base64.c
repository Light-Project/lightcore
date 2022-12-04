/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <initcall.h>
#include <kmalloc.h>
#include <crypto/base64.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: base64 [option] ...\n");
    kshell_printf(ctx, "\t-d  decode data\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state base64_main(struct kshell_context *ctx, int argc, char *argv[])
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
        blen = base64_decode_length(slen);
    else
        blen = base64_encode_length(slen);

    buff = kmalloc(blen, GFP_KERNEL);
    if (!buff)
        return -ENOMEM;

    if (decode)
        base64_decode(buff, argv[count], slen);
    else
        base64_encode(buff, argv[count], slen);

    kshell_printf(ctx, "%.*s\n", (int)blen, buff);
    kfree(buff);

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command base64_cmd = {
    .name = "base64",
    .desc = "base64 encode or decode",
    .exec = base64_main,
};

static state base64_init(void)
{
    return kshell_register(&base64_cmd);
}
kshell_initcall(base64_init);
