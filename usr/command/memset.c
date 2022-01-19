/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "memwrite"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <ctype.h>
#include <string.h>
#include <kernel.h>
#include <initcall.h>
#include <ioremap.h>
#include <kshell.h>
#include <printk.h>

static void usage(void)
{
    printk("usage: memset /nu <addr> <value>        \n");
    printk("  /n - length                           \n");
    printk("  /b - byte (8-bit)                     \n");
    printk("  /h - halfword (16-bit)                \n");
    printk("  /w - word (32-bit)                    \n");
    printk("  /g - giant word (64-bit)              \n");
}

static state memset_main(int argc, char *argv[])
{
    char cstr[20];
    unsigned int count = 0;
    unsigned long long value;
    int num, byte = 4;
    void *addr, *block;
    phys_addr_t phys;

    if (argc == 4 && *argv[1] == '/' && isdigit(*argv[2]) && isdigit(*argv[3])) {
        char *para = argv[1] + 1;

        if (!*para)
            goto usage;

        for (; isdigit(*para); ++count) {
            if (count >= sizeof(cstr))
                goto usage;
            cstr[count] = *para++;
        }

        for (; *para; para++) {
            switch (*para) {
                case 'b':
                    byte = 1;
                    break;

                case 'h':
                    byte = 2;
                    break;

                case 'w':
                    byte = 4;
                    break;

                case 'g':
                    byte = 8;
                    break;

                default:
                    goto usage;
            }
        }
    }

    else if (argc == 3 && isdigit(*argv[1] && isdigit(*argv[2])))
        goto pass;

    else
        goto usage;

pass:
    if (count) {
        num = atoi(cstr);
        if (!num)
            goto usage;
    } else
        num = 1;

    phys = strtoul(argv[argc - 2]);
    value = strtoull(argv[argc - 1]);
    addr = block = ioremap(phys, num * byte);
    if (!addr) {
        pr_err("failed to remapio\n");
        return -ENOMEM;
    }

    while (num--) {
        switch (byte) {
            case 1:
                *(uint8_t *)block = value;
                break;

            case 2:
                *(uint16_t *)block = value;
                break;

            case 4:
                *(uint32_t *)block = value;
                break;

            case 8: default:
                *(uint64_t *)block = value;
                break;
        }
        block += byte;
    }

    iounmap(block);
    return -ENOERR;

usage:
    usage();
    return -EINVAL;
}

static struct kshell_command memset_cmd = {
    .name = "memset",
    .desc = "set memory contents",
    .exec = memset_main,
};

static state mem_init(void)
{
    return kshell_register(&memset_cmd);
}
kshell_initcall(mem_init);
