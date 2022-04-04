/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <string.h>
#include <setjmp.h>
#include <timer.h>
#include <initcall.h>

static void usage(void)
{
    kshell_printf("usage: goto [option] [address]...\n");
    kshell_printf("\t-t  <millisecond> timeout exit function\n");
    kshell_printf("\t-h  display this message\n");
}

static void goto_timeout(void *pdata)
{
    jmp_buf *buff = pdata;
    longjmp(buff, true);
}

static int goto_main(int argc, char *argv[])
{
    state (*program)(int argc, char *argv[]);
    unsigned int count, timeout = 0;
    struct timer *timer;
    jmp_buf buff;
    state retval;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 't':
                if ((++count) >= argc)
                    goto usage;
                timeout = atoi(argv[count]);
                if (!timeout)
                    goto usage;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc == count)
        goto usage;

    program = (void *)strtoul(argv[count]);
    if (!program)
        goto usage;

    if (timeout) {
        timer = timer_create(goto_timeout, &buff, ms_to_ttime(timeout), 0);
        if (!timer) {
            kshell_printf("failed to alloc timer\n");
            return -ENOMEM;
        }
        retval = timer_pending(timer);
        if (retval) {
            kshell_printf("failed to pending timer\n");
            return retval;
        }
        if (setjmp(&buff))
            goto exit;
    }

    kshell_printf("Goto program at (%p)\n", program);
    retval = program(argc - count - 1, &argv[count + 1]);
    kshell_printf("Program terminated, retval (%d)\n", retval);

exit:
    return retval;

usage:
    usage();
    return -EINVAL;
}

static struct kshell_command goto_cmd = {
    .name = "goto",
    .desc = "goto specifies address program",
    .exec = goto_main,
};

static state goto_init(void)
{
    return kshell_register(&goto_cmd);
}
kshell_initcall(goto_init);
