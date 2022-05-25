/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <ctype.h>
#include <string.h>
#include <kernel.h>
#include <initcall.h>
#include <ioops.h>
#include <proc.h>
#include <kshell.h>

static int number_table[] = {
    [0] = 8, [1] = 4, [2] = 4, [4] = 2,
};

static int dec_align_table[] = {
    [0] = 3, [1] = 5, [2] = 11, [4] = 20,
};

static int oct_align_table[] = {
    [0] = 3, [1] = 6, [2] = 11, [4] = 22,
};

enum mem_action {
    ACTION_MEM_READ,
    ACTION_MEM_WRITE,
    ACTION_IOPORT_IN,
    ACTION_IOPORT_OUT,
};

enum write_operator {
    OPERATOR_EQUAL  = 1,
    OPERATOR_ADD    = 2,
    OPERATOR_SUB    = 3,
    OPERATOR_MUL    = 4,
    OPERATOR_DIV    = 5,
    OPERATOR_MOD    = 6,
    OPERATOR_AND    = 7,
    OPERATOR_OR     = 8,
    OPERATOR_XOR    = 9,
    OPERATOR_LEFT   = 10,
    OPERATOR_RIGHT  = 11,
};

enum value_type {
    VALUE_BIT,
    VALUE_BIT_SHIFT,
    VALUE_BIT_RANGE,
};

static enum write_operator get_operator(const char *str)
{
    if (!strcmp(str, "="))
        return OPERATOR_EQUAL;

    else if (!strcmp(str, "+="))
        return OPERATOR_ADD;

    else if (!strcmp(str, "-="))
        return OPERATOR_SUB;

    else if (!strcmp(str, "*="))
        return OPERATOR_MUL;

    else if (!strcmp(str, "/="))
        return OPERATOR_DIV;

    else if (!strcmp(str, "%="))
        return OPERATOR_MOD;

    else if (!strcmp(str, "&="))
        return OPERATOR_AND;

    else if (!strcmp(str, "|="))
        return OPERATOR_OR;

    else if (!strcmp(str, "^="))
        return OPERATOR_XOR;

    else if (!strcmp(str, "<<="))
        return OPERATOR_LEFT;

    else if (!strcmp(str, ">>="))
        return OPERATOR_RIGHT;

    return 0;
}

static uint64_t do_operator(enum write_operator oper, uint64_t va, uint64_t vb)
{
    switch (oper) {
        case OPERATOR_EQUAL:
            va = vb;
            break;

        case OPERATOR_ADD:
            va += vb;
            break;

        case OPERATOR_SUB:
            va -= vb;
            break;

        case OPERATOR_MUL:
            va *= vb;
            break;

        case OPERATOR_DIV:
            if (!vb)
                break;
            va /= vb;
            break;

        case OPERATOR_MOD:
            if (!vb)
                break;
            va %= vb;
            break;

        case OPERATOR_AND:
            va &= vb;
            break;

        case OPERATOR_OR:
            va |= vb;
            break;

        case OPERATOR_XOR:
            va ^= vb;
            break;

        case OPERATOR_LEFT:
            va <<= vb;
            break;

        case OPERATOR_RIGHT:
            va >>= vb;
            break;

        default:
            va = va;
            break;
    }

    return va;
}

static bool legal_val(char *str)
{
    if (*str == '~')
        ++str;

    if (isdigit(*str))
        return true;

    if (!strncmp(str, "BIT(", 4))
        return true;

    if (!strncmp(str, "SHIFT(", 6))
        return true;

    if (!strncmp(str, "RANGE(", 6))
        return true;

    return false;
}

static uint64_t convert_val(char *str)
{
    enum value_type type;
    unsigned long long va;
    unsigned long vb;
    char *tmp;
    bool reversal = false;

    if (*str == '~') {
        reversal = true;
        ++str;
    }

    if (!strncmp(str, "BIT(", 4)) {
        type = VALUE_BIT;
        str += 4;
        va = 0;
        goto skip_va;
    }

    else if (!strncmp(str, "SHIFT(", 6)) {
        type = VALUE_BIT_SHIFT;
        str += 6;
    }

    else if (!strncmp(str, "RANGE(", 6)) {
        type = VALUE_BIT_RANGE;
        str += 6;
    }

    else {
        va = axtoull(str);
        goto skip_type;
    }

    tmp = strchr(str, ',');
    if (!tmp || tmp == str)
        return 0;
    va = axtoull(str);
    str = tmp + 1;

skip_va:
    tmp = strchr(str, ')');
    if (!tmp || tmp == str)
        return 0;
    vb = axtoul(str);

    switch (type) {
        case VALUE_BIT:
            va = BIT_ULL(vb);
            break;

        case VALUE_BIT_SHIFT:
            va = BIT_SHIFT_ULL(va, vb);
            break;

        case VALUE_BIT_RANGE:
            va = BIT_RANGE_ULL(va, vb);
            break;

        default:
            va = va;
    }

skip_type:
    return reversal ? ~va : va;
}

static void mem_read_usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: mem -r /nfu <addr>\n");
    kshell_printf(ctx, "\t/n - length\n");
    kshell_printf(ctx, "\t/o - octal\n");
    kshell_printf(ctx, "\t/x - hexadecimal\n");
    kshell_printf(ctx, "\t/d - decimal\n");
    kshell_printf(ctx, "\t/u - unsigned decimal\n");
    kshell_printf(ctx, "\t/a - address\n");
    kshell_printf(ctx, "\t/c - char\n");
    kshell_printf(ctx, "\t/b - byte (8-bit)\n");
    kshell_printf(ctx, "\t/h - halfword (16-bit)\n");
    kshell_printf(ctx, "\t/w - word (32-bit)\n");
    kshell_printf(ctx, "\t/g - giant word (64-bit)\n");
}

static state mem_read(struct kshell_context *ctx, int argc, char *argv[])
{
    char buff[10], cstr[20] = {}, fmt = 'x';
    unsigned int count = 0;
    int num, byte = 4;
    void *addr, *block;
    phys_addr_t phys;

    if (argc == 2 && *argv[0] == '/' && isdigit(*argv[1])) {
        char *para = argv[0] + 1;

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

    else if (argc == 1 && isdigit(*argv[0]))
        goto pass;

    else
        goto usage;

pass:
    if (fmt == 'x') {
        sprintf(buff, "0x%%0%d%s%c", byte * 2,
                byte == 8 ? "ll" : "", fmt);
    } else {
        sprintf(buff, "%%-%d%s%c", (fmt == 'o' ?
                oct_align_table : dec_align_table)[byte / 2],
                byte == 8 ? "ll" : "", fmt);
    }

exit:
    if (count) {
        num = atoi(cstr);
        if (!num)
            goto usage;
    } else
        num = 1;

    phys = axtoul(argv[argc - 1]);
    addr = block = ioremap(phys, num * byte);
    if (!addr) {
        kshell_printf(ctx, "failed to remapio\n");
        return -ENOMEM;
    }

    for (; (count = min(num, number_table[byte / 2])); num -= count) {
        unsigned int tmp;
        kshell_printf(ctx, "0x%08lx: ", phys);
        for (tmp = 0; tmp < count; ++tmp) {
            switch (byte) {
                case 1:
                    kshell_printf(ctx, buff, readb(addr), readb(addr));
                    break;
                case 2:
                    kshell_printf(ctx, buff, unaligned_get_u16(addr));
                    break;
                case 4:
                    kshell_printf(ctx, buff, unaligned_get_u32(addr));
                    break;
                case 8: default:
                    kshell_printf(ctx, buff, unaligned_get_u64(addr));
                    break;
            }
            phys += byte;
            addr += byte;
            kshell_printf(ctx, "  ");
        }
        kshell_printf(ctx, "\n");
    }

    iounmap(block);
    return -ENOERR;

usage:
    mem_read_usage(ctx);
    return -EINVAL;
}

static void operator_value_usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "Operator types:\n");
    kshell_printf(ctx, "\t+=  - add target memory\n");
    kshell_printf(ctx, "\t-=  - sub target memory\n");
    kshell_printf(ctx, "\t*=  - mul target memory\n");
    kshell_printf(ctx, "\t/=  - div target memory\n");
    kshell_printf(ctx, "\t%%=  - mod target memory\n");
    kshell_printf(ctx, "\t&=  - and target memory\n");
    kshell_printf(ctx, "\t|=  - or  target memory\n");
    kshell_printf(ctx, "\t^=  - xor target memory\n");
    kshell_printf(ctx, "\t<<= - shl target memory\n");
    kshell_printf(ctx, "\t>>= - shr target memory\n");
    kshell_printf(ctx, "Value types:\n");
    kshell_printf(ctx, "\tBIT(shift)       - create a bitmask\n");
    kshell_printf(ctx, "\tSHIFT(shift,val) - create a shifted bitmask\n");
    kshell_printf(ctx, "\tRANGE(high,low)  - create a contiguous bitmask\n");
    kshell_printf(ctx, "\t~expression      - bit inversion\n");
    kshell_printf(ctx, "Expression example:\n");
    kshell_printf(ctx, "\taddr &= ~RANGE(7,0)    - 0x000012ff -> 0x00001200\n");
    kshell_printf(ctx, "\taddr <<= 16            - 0x00001200 -> 0x12000000\n");
    kshell_printf(ctx, "\taddr |= SHIFT(16,0x34) - 0x12000000 -> 0x12340000\n");
    kshell_printf(ctx, "\taddr += 0x5670         - 0x12340000 -> 0x12345670\n");
    kshell_printf(ctx, "\taddr |= BIT(3)         - 0x12345670 -> 0x12345678\n");
}

static void mem_write_usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: mem -w /nu <addr> <operator> <value>\n");
    kshell_printf(ctx, "\t/n - length\n");
    kshell_printf(ctx, "\t/b - byte (8-bit)\n");
    kshell_printf(ctx, "\t/h - halfword (16-bit)\n");
    kshell_printf(ctx, "\t/w - word (32-bit)\n");
    kshell_printf(ctx, "\t/g - giant word (64-bit)\n");
    operator_value_usage(ctx);
}

static state mem_write(struct kshell_context *ctx, int argc, char *argv[])
{
    char cstr[20] = {};
    unsigned int count = 0;
    unsigned long long value, tmp;
    int num, byte = 4;
    enum write_operator oper;
    void *addr, *block;
    phys_addr_t phys;

    if (argc == 4 && *argv[0] == '/' && isdigit(*argv[1]) && legal_val(argv[3])) {
        char *para = argv[0] + 1;

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

                case 'c': case 'a':
                case 'x': case 'd':
                case 'u': case 'o':
                    break;

                default:
                    goto usage;
            }
        }
    }

    else if (argc == 3 && isdigit(*argv[0]) && legal_val(argv[2]))
        goto pass;

    else
        goto usage;

pass:
    oper = get_operator(argv[argc - 2]);
    if (!oper)
        goto usage;

    if (count) {
        num = atoi(cstr);
        if (!num)
            goto usage;
    } else
        num = 1;

    phys = axtoul(argv[argc - 3]);
    value = convert_val(argv[argc - 1]);
    addr = block = ioremap(phys, num * byte);
    if (!addr) {
        kshell_printf(ctx, "failed to remapio\n");
        return -ENOMEM;
    }

    while (num--) {
        switch (byte) {
            case 1:
                tmp = readb(addr);
                tmp = do_operator(oper, tmp, (uint8_t)value);
                writeb(addr, tmp);
                break;

            case 2:
                tmp = unaligned_get_u16(addr);
                tmp = do_operator(oper, tmp, (uint16_t)value);
                unaligned_set_u16(addr, tmp);
                break;

            case 4:
                tmp = unaligned_get_u32(addr);
                tmp = do_operator(oper, tmp, (uint32_t)value);
                unaligned_set_u32(addr, tmp);
                break;

            case 8: default:
                tmp = unaligned_get_u64(addr);
                tmp = do_operator(oper, tmp, (uint64_t)value);
                unaligned_set_u64(addr, tmp);
                break;
        }
        addr += byte;
    }

    iounmap(block);
    return -ENOERR;

usage:
    mem_write_usage(ctx);
    return -EINVAL;
}

static void ioport_in_usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: mem -i /nfu <addr>\n");
    kshell_printf(ctx, "\t/n - length\n");
    kshell_printf(ctx, "\t/o - octal\n");
    kshell_printf(ctx, "\t/x - hexadecimal\n");
    kshell_printf(ctx, "\t/d - decimal\n");
    kshell_printf(ctx, "\t/u - unsigned decimal\n");
    kshell_printf(ctx, "\t/a - address\n");
    kshell_printf(ctx, "\t/c - char\n");
    kshell_printf(ctx, "\t/b - byte (8-bit)\n");
    kshell_printf(ctx, "\t/h - halfword (16-bit)\n");
    kshell_printf(ctx, "\t/w - word (32-bit)\n");
}

static state ioport_in(struct kshell_context *ctx, int argc, char *argv[])
{
    char buff[10], cstr[20] = {}, fmt = 'x';
    unsigned int count = 0;
    int num, byte = 1;
    resource_size_t base;

    if (argc == 2 && *argv[0] == '/' && isdigit(*argv[1])) {
        char *para = argv[0] + 1;

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

    else if (argc == 1 && isdigit(*argv[0]))
        goto pass;

    else
        goto usage;

pass:
    if (fmt == 'x')
        sprintf(buff, "0x%%0%d%c", byte * 2, fmt);
    else {
        sprintf(buff, "%%-%d%c", (fmt == 'o' ?
                oct_align_table : dec_align_table)[byte / 2], fmt);
    }

exit:
    if (count) {
        num = atoi(cstr);
        if (!num)
            goto usage;
    } else
        num = 1;

    base = axtoul(argv[argc - 1]);

    for (; (count = min(num, number_table[byte / 2])); num -= count) {
        unsigned int tmp;
        kshell_printf(ctx, "0x%04lx: ", base);
        for (tmp = 0; tmp < count; ++tmp) {
            switch (byte) {
                case 1:
                    kshell_printf(ctx, buff, inb(base), inb(base));
                    break;
                case 2:
                    kshell_printf(ctx, buff, inw(base));
                    break;
                case 4: default:
                    kshell_printf(ctx, buff, inl(base));
                    break;
            }
            base += byte;
            kshell_printf(ctx, "  ");
        }
        kshell_printf(ctx, "\n");
    }

    return -ENOERR;

usage:
    ioport_in_usage(ctx);
    return -EINVAL;
}

static void ioport_out_usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: mem -o /nu <addr> <operator> <value>\n");
    kshell_printf(ctx, "\t/n - length\n");
    kshell_printf(ctx, "\t/b - byte (8-bit)\n");
    kshell_printf(ctx, "\t/h - halfword (16-bit)\n");
    kshell_printf(ctx, "\t/w - word (32-bit)\n");
    operator_value_usage(ctx);
}

static state ioport_out(struct kshell_context *ctx, int argc, char *argv[])
{
    char cstr[20] = {};
    unsigned int count = 0;
    unsigned long long value, tmp;
    enum write_operator oper;
    int num, byte = 1;
    phys_addr_t base;

    if (argc == 4 && *argv[0] == '/' && isdigit(*argv[1]) && legal_val(argv[3])) {
        char *para = argv[0] + 1;

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

                case 'c': case 'a':
                case 'x': case 'd':
                case 'u': case 'o':
                    break;

                default:
                    goto usage;
            }
        }
    }

    else if (argc == 3 && isdigit(*argv[0]) && legal_val(argv[2]))
        goto pass;

    else
        goto usage;

pass:
    oper = get_operator(argv[argc - 2]);
    if (!oper)
        goto usage;

    if (count) {
        num = atoi(cstr);
        if (!num)
            goto usage;
    } else
        num = 1;

    base = axtoul(argv[argc - 3]);
    value = convert_val(argv[argc - 1]);

    while (num--) {
        switch (byte) {
            case 1:
                tmp = inb(base);
                tmp = do_operator(oper, tmp, (uint8_t)value);
                outb(base, (uint8_t)value);
                break;

            case 2:
                tmp = inw(base);
                tmp = do_operator(oper, tmp, (uint16_t)value);
                outw(base, (uint16_t)value);
                break;

            case 4: default:
                tmp = inl(base);
                tmp = do_operator(oper, tmp, (uint16_t)value);
                outl(base, (uint32_t)value);
                break;
        }
        base += byte;
    }

    return -ENOERR;

usage:
    ioport_out_usage(ctx);
    return -EINVAL;
}

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: mem [option] ...\n");
    kshell_printf(ctx, "\t-r  memory read (default)\n");
    kshell_printf(ctx, "\t-w  memory write\n");
    kshell_printf(ctx, "\t-i  iomem input\n");
    kshell_printf(ctx, "\t-o  iomem output\n");
    kshell_printf(ctx, "\t-s  show after modification\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state mem_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int act = 0;
    bool show = false;
    state ret;

    if (argc < 2)
        goto usage;

    else if (*argv[1] == '/' || isdigit(*argv[1]))
        return mem_read(ctx, argc - 1, &argv[1]);

    else if (*argv[1] == '-') {
        char *para = argv[1] + 1;

        if (!*para)
            goto usage;

        for (; *para; para++) {
            switch (*para) {
                case 'r':
                    act = ACTION_MEM_READ;
                    break;

                case 'w':
                    act = ACTION_MEM_WRITE;
                    break;

                case 'i':
                    act = ACTION_IOPORT_IN;
                    break;

                case 'o':
                    act = ACTION_IOPORT_OUT;
                    break;

                case 's':
                    show = true;
                    break;

                case 'h': default:
                    goto usage;
            }
        }
    }

    else
        goto usage;

    switch (act) {
        case ACTION_MEM_WRITE:
            ret = mem_write(ctx, argc - 2, &argv[2]);
            if (ret || !show)
                return ret;
            show = false;
            argc -= 2;

        case ACTION_MEM_READ: default:
            if (show)
                goto usage;
            return mem_read(ctx, argc - 2, &argv[2]);

        case ACTION_IOPORT_OUT:
            ret = ioport_out(ctx, argc - 2, &argv[2]);
            if (ret || !show)
                return ret;
            show = false;
            argc -= 2;

        case ACTION_IOPORT_IN:
            if (show)
                goto usage;
            return ioport_in(ctx, argc - 2, &argv[2]);
    }

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command mem_cmd = {
    .name = "mem",
    .desc = "memory toolset",
    .exec = mem_main,
};

static state mem_init(void)
{
    return kshell_register(&mem_cmd);
}
kshell_initcall(mem_init);
