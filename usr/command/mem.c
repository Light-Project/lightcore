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
#include <driver/pci.h>
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
    ACTION_PCIDEV_READ,
    ACTION_PCIDEV_WRITE,
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
    kshell_puts(ctx, "usage: mem -r /nfu <addr>\n");
    kshell_puts(ctx, "\t/n - length\n");
    kshell_puts(ctx, "\t/o - octal\n");
    kshell_puts(ctx, "\t/x - hexadecimal\n");
    kshell_puts(ctx, "\t/d - decimal\n");
    kshell_puts(ctx, "\t/u - unsigned decimal\n");
    kshell_puts(ctx, "\t/a - address\n");
    kshell_puts(ctx, "\t/c - char\n");
    kshell_puts(ctx, "\t/b - byte (8-bit)\n");
    kshell_puts(ctx, "\t/h - halfword (16-bit)\n");
    kshell_puts(ctx, "\t/w - word (32-bit)\n");
    kshell_puts(ctx, "\t/g - giant word (64-bit)\n");
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
        kshell_puts(ctx, "failed to remapio\n");
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
            kshell_puts(ctx, "  ");
        }
        kshell_putc(ctx, '\n');
    }

    iounmap(block);
    return -ENOERR;

usage:
    mem_read_usage(ctx);
    return -EINVAL;
}

static void operator_value_usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "Operator types:\n");
    kshell_puts(ctx, "\t+=  - add target memory\n");
    kshell_puts(ctx, "\t-=  - sub target memory\n");
    kshell_puts(ctx, "\t*=  - mul target memory\n");
    kshell_puts(ctx, "\t/=  - div target memory\n");
    kshell_puts(ctx, "\t%%=  - mod target memory\n");
    kshell_puts(ctx, "\t&=  - and target memory\n");
    kshell_puts(ctx, "\t|=  - or  target memory\n");
    kshell_puts(ctx, "\t^=  - xor target memory\n");
    kshell_puts(ctx, "\t<<= - shl target memory\n");
    kshell_puts(ctx, "\t>>= - shr target memory\n");
    kshell_puts(ctx, "Value types:\n");
    kshell_puts(ctx, "\tBIT(shift)       - create a bitmask\n");
    kshell_puts(ctx, "\tSHIFT(shift,val) - create a shifted bitmask\n");
    kshell_puts(ctx, "\tRANGE(high,low)  - create a contiguous bitmask\n");
    kshell_puts(ctx, "\t~expression      - bit inversion\n");
    kshell_puts(ctx, "Expression example:\n");
    kshell_puts(ctx, "\taddr &= ~RANGE(7,0)    - 0x000012ff -> 0x00001200\n");
    kshell_puts(ctx, "\taddr <<= 16            - 0x00001200 -> 0x12000000\n");
    kshell_puts(ctx, "\taddr |= SHIFT(16,0x34) - 0x12000000 -> 0x12340000\n");
    kshell_puts(ctx, "\taddr += 0x5670         - 0x12340000 -> 0x12345670\n");
    kshell_puts(ctx, "\taddr |= BIT(3)         - 0x12345670 -> 0x12345678\n");
}

static void mem_write_usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: mem -w /nu <addr> <operator> <value>\n");
    kshell_puts(ctx, "\t/n - length\n");
    kshell_puts(ctx, "\t/b - byte (8-bit)\n");
    kshell_puts(ctx, "\t/h - halfword (16-bit)\n");
    kshell_puts(ctx, "\t/w - word (32-bit)\n");
    kshell_puts(ctx, "\t/g - giant word (64-bit)\n");
    operator_value_usage(ctx);
}

static state mem_write(struct kshell_context *ctx, int argc, char *argv[])
{
    char cstr[20] = {};
    unsigned int count = 0;
    uint64_t value, tmp;
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
        kshell_puts(ctx, "failed to remapio\n");
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

#ifdef CONFIG_ARCH_HAS_PMIO
static void ioport_in_usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: mem -i /nfu <addr>\n");
    kshell_puts(ctx, "\t/n - length\n");
    kshell_puts(ctx, "\t/o - octal\n");
    kshell_puts(ctx, "\t/x - hexadecimal\n");
    kshell_puts(ctx, "\t/d - decimal\n");
    kshell_puts(ctx, "\t/u - unsigned decimal\n");
    kshell_puts(ctx, "\t/a - address\n");
    kshell_puts(ctx, "\t/c - char\n");
    kshell_puts(ctx, "\t/b - byte (8-bit)\n");
    kshell_puts(ctx, "\t/h - halfword (16-bit)\n");
    kshell_puts(ctx, "\t/w - word (32-bit)\n");
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
            kshell_puts(ctx, "  ");
        }
        kshell_putc(ctx, '\n');
    }

    return -ENOERR;

usage:
    ioport_in_usage(ctx);
    return -EINVAL;
}

static void ioport_out_usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: mem -o /nu <addr> <operator> <value>\n");
    kshell_puts(ctx, "\t/n - length\n");
    kshell_puts(ctx, "\t/b - byte (8-bit)\n");
    kshell_puts(ctx, "\t/h - halfword (16-bit)\n");
    kshell_puts(ctx, "\t/w - word (32-bit)\n");
    operator_value_usage(ctx);
}

static state ioport_out(struct kshell_context *ctx, int argc, char *argv[])
{
    char cstr[20] = {};
    unsigned int count = 0;
    uint32_t value, tmp;
    enum write_operator oper;
    int num, byte = 1;
    resource_size_t base;

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
    value = (uint32_t)convert_val(argv[argc - 1]);

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
                tmp = do_operator(oper, tmp, value);
                outl(base, value);
                break;
        }
        base += byte;
    }

    return -ENOERR;

usage:
    ioport_out_usage(ctx);
    return -EINVAL;
}
#endif

#ifdef CONFIG_PCI
static void pcidev_read_usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: mem -p domain:bus:slot.func /nfu <addr>\n");
    kshell_puts(ctx, "\t/n - length\n");
    kshell_puts(ctx, "\t/o - octal\n");
    kshell_puts(ctx, "\t/x - hexadecimal\n");
    kshell_puts(ctx, "\t/d - decimal\n");
    kshell_puts(ctx, "\t/u - unsigned decimal\n");
    kshell_puts(ctx, "\t/a - address\n");
    kshell_puts(ctx, "\t/c - char\n");
    kshell_puts(ctx, "\t/b - byte (8-bit)\n");
    kshell_puts(ctx, "\t/h - halfword (16-bit)\n");
    kshell_puts(ctx, "\t/w - word (32-bit)\n");
}

static state pcidev_read(struct kshell_context *ctx, int argc, char *argv[])
{
    char buff[10], cstr[20] = {}, fmt = 'x';
    unsigned int domain, bus, slot, func;
    unsigned int base, count = 0;
    int num, byte = 1;

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

    if (sscanf(argv[0], "%u:%u:%u.%u", &domain, &bus, &slot, &func) < 0)
        goto usage;
    base = axtoui(argv[argc - 1]);

    for (; (count = min(num, number_table[byte / 2])); num -= count) {
        unsigned int tmp;
        kshell_printf(ctx, "0x%04x: ", base);
        for (tmp = 0; tmp < count; ++tmp) {
            uint32_t value;
            pci_raw_config_read(domain, bus, PCI_DEVFN(slot, func), base, byte, &value);
            kshell_printf(ctx, buff, value, value);
            base += byte;
            kshell_puts(ctx, "  ");
        }
        kshell_putc(ctx, '\n');
    }

    return -ENOERR;

usage:
    pcidev_read_usage(ctx);
    return -EINVAL;
}

static void pcidev_write_usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: mem -o domain:bus:slot.func /nu <addr> <operator> <value>\n");
    kshell_puts(ctx, "\t/n - length\n");
    kshell_puts(ctx, "\t/b - byte (8-bit)\n");
    kshell_puts(ctx, "\t/h - halfword (16-bit)\n");
    kshell_puts(ctx, "\t/w - word (32-bit)\n");
    operator_value_usage(ctx);
}

static state pcidev_write(struct kshell_context *ctx, int argc, char *argv[])
{
    char cstr[20] = {};
    unsigned int domain, bus, slot, func;
    unsigned int base, count = 0;
    uint32_t value, tmp;
    enum write_operator oper;
    int num, byte = 1;

    if (argc == 5 && *argv[1] == '/' && isdigit(*argv[2]) && legal_val(argv[4])) {
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

                case 'c': case 'a':
                case 'x': case 'd':
                case 'u': case 'o':
                    break;

                default:
                    goto usage;
            }
        }
    }

    else if (argc == 4 && isdigit(*argv[1]) && legal_val(argv[3]))
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

    if (sscanf(argv[0], "%u:%u:%u.%u", &domain, &bus, &slot, &func) < 0)
        goto usage;
    base = axtou32(argv[argc - 3]);
    value = (uint32_t)convert_val(argv[argc - 1]);

    while (num--) {
        pci_raw_config_read(domain, bus, PCI_DEVFN(slot, func), base, byte, &tmp);
        tmp = do_operator(oper, tmp, value & BIT_LOW_MASK(BITS_PER_BYTE * byte));
        pci_raw_config_write(domain, bus, PCI_DEVFN(slot, func), base, byte, tmp);
        base += byte;
    }

    return -ENOERR;

usage:
    pcidev_write_usage(ctx);
    return -EINVAL;
}
#endif

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: mem [option] ...\n");
    kshell_puts(ctx, "\t-r  memory read (default)\n");
    kshell_puts(ctx, "\t-w  memory write\n");
#ifdef CONFIG_ARCH_HAS_PMIO
    kshell_puts(ctx, "\t-i  ioport input\n");
    kshell_puts(ctx, "\t-o  ioport output\n");
#endif
#ifdef CONFIG_PCI
    kshell_puts(ctx, "\t-p  pci configuration read\n");
    kshell_puts(ctx, "\t-q  pci configuration write\n");
#endif
    kshell_puts(ctx, "\t-s  show after modification\n");
    kshell_puts(ctx, "\t-h  display this message\n");
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

#ifdef CONFIG_ARCH_HAS_PMIO
                case 'i':
                    act = ACTION_IOPORT_IN;
                    break;

                case 'o':
                    act = ACTION_IOPORT_OUT;
                    break;
#endif

#ifdef CONFIG_PCI
                case 'p':
                    act = ACTION_PCIDEV_READ;
                    break;

                case 'q':
                    act = ACTION_PCIDEV_WRITE;
                    break;
#endif

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
            fallthrough;

        case ACTION_MEM_READ: default:
            if (show)
                goto usage;
            return mem_read(ctx, argc - 2, &argv[2]);

#ifdef CONFIG_ARCH_HAS_PMIO
        case ACTION_IOPORT_OUT:
            ret = ioport_out(ctx, argc - 2, &argv[2]);
            if (ret || !show)
                return ret;
            show = false;
            argc -= 2;
            fallthrough;

        case ACTION_IOPORT_IN:
            if (show)
                goto usage;
            return ioport_in(ctx, argc - 2, &argv[2]);
#endif

#ifdef CONFIG_PCI
        case ACTION_PCIDEV_WRITE:
            ret = pcidev_write(ctx, argc - 2, &argv[2]);
            if (ret || !show)
                return ret;
            show = false;
            argc -= 2;
            fallthrough;

        case ACTION_PCIDEV_READ:
            if (show)
                goto usage;
            return pcidev_read(ctx, argc - 2, &argv[2]);
#endif
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
