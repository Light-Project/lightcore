/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <initcall.h>
#include <kmalloc.h>
#include <crypto/ascii85.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: ascii85 [option] ...\n");
    kshell_printf(ctx, "\t-d  decode data\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state ascii85_main(struct kshell_context *ctx, int argc, char *argv[])
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
        blen = ascii85_decode_length(argv[count], slen);
    else
        blen = ascii85_encode_length(argv[count], slen);

    buff = kmalloc(blen, GFP_KERNEL);
    if (!buff)
        return -ENOMEM;

    if (decode)
        ascii85_decode(buff, argv[count], slen);
    else
        ascii85_encode(buff, argv[count], slen);

    kshell_printf(ctx, "%.*s\n", (int)blen, buff);
    kfree(buff);

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command ascii85_cmd = {
    .name = "ascii85",
    .desc = "ascii85 encode or decode",
    .exec = ascii85_main,
};

static state ascii85_init(void)
{
    return kshell_register(&ascii85_cmd);
}
kshell_initcall(ascii85_init);
