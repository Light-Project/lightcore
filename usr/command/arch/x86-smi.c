/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <string.h>
#include <kshell.h>
#include <proc.h>
#include <asm/smi.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: x86-smi [option] reg=value ...\n");
    kshell_printf(ctx, "\t-q  quiet mode outputs nothing\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state smi_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct smi_regs regs = {};
    unsigned int count;
    bool quiet = false;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'q':
                quiet = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc <= count)
        goto usage;

    do {
        char *equals;

        equals = strchr(argv[count], '=');
        if (!equals)
            goto usage;

        *equals++ = '\0';
        if (equals - argv[count] != 4)
            goto usage;

        switch (unaligned_get_le32(argv[count])) {
            case 0x786165: /* eax */
                regs.eax = axtou32(equals);
                break;

            case 0x786265: /* ebx */
                regs.ebx = axtou32(equals);
                break;

            case 0x786365: /* ecx */
                regs.ecx = axtou32(equals);
                break;

            case 0x786465: /* edx */
                regs.edx = axtou32(equals);
                break;

            case 0x697365: /* esi */
                regs.esi = axtou32(equals);
                break;

            case 0x696465: /* edi */
                regs.edi = axtou32(equals);
                break;

            default:
                goto usage;
        }
    } while (++count < argc);

    smi_trigger(&regs);

    if (!quiet) {
        kshell_printf(ctx,
            "eax=%#010x ebx=%#010x ecx=%#010x\n"
            "edx=%#010x esi=%#010x edi=%#010x\n",
            regs.eax, regs.ebx, regs.ecx,
            regs.edx, regs.esi, regs.edi
        );
    }

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command smi_cmd = {
    .name = "x86-smi",
    .desc = "show processor smi",
    .exec = smi_main,
};

static state smi_init(void)
{
    return kshell_register(&smi_cmd);
}
kshell_initcall(smi_init);
