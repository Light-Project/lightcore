/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "mem"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <ctype.h>
#include <string.h>
#include <kernel.h>
#include <initcall.h>
#include <ioremap.h>
#include <kshell.h>
#include <printk.h>

static int align_table[] = {
    [0] = 3, [1] = 6, [2] = 11, [4] = 16,
};

static void usage(void)
{
    printk("usage: mem /nfu <addr>                  \n");
    printk("  /n - length                           \n");
    printk("  /o - octal                            \n");
    printk("  /x - hexadecimal                      \n");
    printk("  /d - decimal                          \n");
    printk("  /u - unsigned decimal                 \n");
    printk("  /a - address                          \n");
    printk("  /c - char                             \n");
    printk("  /b - byte (8-bit)                     \n");
    printk("  /h - halfword (16-bit)                \n");
    printk("  /w - word (32-bit)                    \n");
    printk("  /g - giant word (64-bit)              \n");
}

static state mem_main(int argc, char *argv[])
{
    char buff[10], cstr[20], fmt = 'x';
    unsigned int count = 0;
    int num, byte = 4;
    void *addr, *block;
    phys_addr_t phys;

    if (argc == 3 && *argv[1] == '/' && isdigit(*argv[2])) {
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

                case 'c':
                    byte = 1;
                    strcpy(buff, "'\\%d' '%c'");
                    goto exit;

                case 'a':
                    fmt = 'x';
                    break;

                case 'x': case 'd':
                case 'u': case 'o':
                    fmt = *para;
                    break;

                default:
                    goto usage;
            }
        }
    }

    else if (argc == 2 && isdigit(*argv[1]))
        goto pass;

    else
        goto usage;

pass:
    if (fmt == 'x')
        sprintf(buff, "0x%%0%d%c", byte * 2, fmt);
    else
        sprintf(buff, "%%-%d%c", align_table[byte / 2], fmt);

exit:
    if (count) {
        num = atoi(cstr);
        if (!num)
            goto usage;
    } else
        num = 1;

    phys = strtoul(argv[argc - 1]);
    addr = block = ioremap(phys, num * byte);
    if (!addr) {
        pr_err("failed to remapio\n");
        return -ENOMEM;
    }

    for (; (count = min(num, 4)); num -= count) {
        unsigned int tmp;
        printk("0x%08lx: ", phys);
        for (tmp = 0; tmp < count; ++tmp) {
            switch (byte) {
                case 1:
                    printk(buff, *(uint8_t *)addr, *(uint8_t *)addr);
                    break;
                case 2:
                    printk(buff, *(uint16_t *)addr);
                    break;
                case 4:
                    printk(buff, *(uint32_t *)addr);
                    break;
                case 8: default:
                    printk(buff, *(uint64_t *)addr);
                    break;
            }
            phys += byte;
            addr += byte;
            printk("  ");
        }
        printk("\n");
    }

    iounmap(block);
    return -ENOERR;

usage:
    usage();
    return -EINVAL;
}

static struct kshell_command mem_cmd = {
    .name = "mem",
    .desc = "dump memory contents",
    .exec = mem_main,
};

static state mem_init(void)
{
    return kshell_register(&mem_cmd);
}
kshell_initcall(mem_init);
