/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kshell.h>
#include <asm/regs.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: x86-msr [option] opcode value\n");
    kshell_puts(ctx, "\t-w  write msr register\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state msr_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;
    bool wflag = false;
    uint64_t value;
    unsigned int opcode;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'w':
                wflag = true;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    opcode = axtou64(argv[count++]);
    if (wflag) {
        value = axtou64(argv[count]);
        msr_set(opcode, value);
    }

    value = msr_get(opcode);
    kshell_printf(ctx, "%#llx\n", value);

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command msr_cmd = {
    .name = "x86-msr",
    .desc = "show processor msr",
    .exec = msr_main,
};

static state msr_init(void)
{
    return kshell_register(&msr_cmd);
}
kshell_initcall(msr_init);
