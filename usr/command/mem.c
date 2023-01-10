/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <ctype.h>
#include <string.h>
#include <kernel.h>
#include <initcall.h>
#include <numalign.h>
#include <ioops.h>
#include <proc.h>
#include <driver/pci.h>
#include <kshell.h>

static unsigned int number_table[] = {
    [0] = 8, [1] = 4, [2] = 4, [4] = 2,
};

enum mem_action {
    ACTION_MEM,
    ACTION_VIRT,
    ACTION_IOPORT,
    ACTION_PCICFG,
};

enum write_operator {
    OPERATOR_NULL,
    OPERATOR_EQUAL,
    OPERATOR_ADD,
    OPERATOR_SUB,
    OPERATOR_MUL,
    OPERATOR_DIV,
    OPERATOR_MOD,
    OPERATOR_AND,
    OPERATOR_OR,
    OPERATOR_XOR,
    OPERATOR_LEFT,
    OPERATOR_RIGHT,
};

enum value_type {
    VALUE_RAW,
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

    return OPERATOR_NULL;
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

static bool convert_val(char *str, uint64_t *valp)
{
    enum value_type type = VALUE_RAW;
    bool reversal = false;
    unsigned long long va;
    unsigned long vb;
    char *tmp;

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
        if (!isdigit(*str)) {
            errno = -EINVAL;
            return false;
        }

        va = axtoull(str);
        goto skip_type;
    }

    tmp = strchr(str, ',');
    if (!tmp || tmp == str)
        return false;

    va = axtoull(str);
    str = tmp + 1;

skip_va:
    tmp = strchr(str, ')');
    if (!tmp || tmp == str)
        return false;

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
    *valp = reversal ? ~va : va;
    return true;
}

static void operator_value_usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "Operator types:\n");
    kshell_puts(ctx, "\t+=  - add target memory\n");
    kshell_puts(ctx, "\t-=  - sub target memory\n");
    kshell_puts(ctx, "\t*=  - mul target memory\n");
    kshell_puts(ctx, "\t/=  - div target memory\n");
    kshell_puts(ctx, "\t%=  - mod target memory\n");
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

static void mem_usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: mem -m /nfu <addr> [<operator> <value>]\n");
    kshell_puts(ctx, "\t/n - length\n");
    kshell_puts(ctx, "Format modifiers:\n");
    kshell_puts(ctx, "\t/o - octal\n");
    kshell_puts(ctx, "\t/x - hexadecimal\n");
    kshell_puts(ctx, "\t/d - decimal\n");
    kshell_puts(ctx, "\t/u - unsigned decimal\n");
    kshell_puts(ctx, "Unit modifiers:\n");
    kshell_puts(ctx, "\t/c - char\n");
    kshell_puts(ctx, "\t/b - byte (8-bit)\n");
    kshell_puts(ctx, "\t/h - halfword (16-bit)\n");
    kshell_puts(ctx, "\t/w - word (32-bit)\n");
    kshell_puts(ctx, "\t/g - giant word (64-bit)\n");
    operator_value_usage(ctx);
}

static state mem_read(struct kshell_context *ctx, int argc, char *argv[], bool virtual)
{
    char buff[10], fmt = 'x';
    unsigned int num = 1, byte = 4;
    unsigned int align, count;
    void *addr, *block;
    unsigned long index;

    if (argc == 2 && *argv[0] == '/' && isdigit(*argv[1])) {
        char *para = argv[0] + 1;

        if (!*para)
            goto usage;

        if (isdigit(*para))
            num = strtoui(para, &para, 0);

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
    switch (fmt) {
        case 'o':
            align = oct_align(byte);
            break;

        case 'd':
            align = dec_signed_align(byte);
            break;

        case 'u':
            align = dec_unsigned_align(byte);
            break;

        case 'x':
            align = byte * 2 + 2;
            break;

        default:
            goto usage;
    }

    if (fmt == 'x') {
        sprintf(buff, "%%#0%d%s%c", align,
                byte == 8 ? "ll" : "", fmt);
    } else {
        sprintf(buff, "%%-%d%s%c", align,
                byte == 8 ? "ll" : "", fmt);
    }

exit:
    if (virtual) {
        index = axtoul(argv[argc - 1]);
        addr = block = (void *)index;
        if (!addr) {
            kshell_puts(ctx, "illegal virtual address\n");
            return -ENOMEM;
        }
    } else {
        index = axtoul(argv[argc - 1]);
        addr = block = ioremap(index, num * byte);

        if (!addr) {
            kshell_puts(ctx, "ioremap failed\n");
            return -ENOMEM;
        }
    }

    for (; (count = min(num, number_table[byte / 2])); num -= count) {
        unsigned int tmp;
        kshell_printf(ctx, "%#010lx: ", index);
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
            index += byte;
            addr += byte;
            kshell_putc(ctx, ' ');
        }
        kshell_putc(ctx, '\n');
    }

    if (!virtual)
        iounmap(block);

    return -ENOERR;

usage:
    mem_usage(ctx);
    return -EINVAL;
}

static state mem_write(struct kshell_context *ctx, int argc, char *argv[],
                       enum write_operator oper, bool virtual)
{
    unsigned int num = 1, byte = 4;
    void *addr, *block;
    uint64_t value;

    if (argc == 4 && *argv[0] == '/' && isdigit(*argv[1])) {
        char *para = argv[0] + 1;

        if (!*para || !convert_val(argv[3], &value))
            goto usage;

        if (isdigit(*para))
            num = strtoui(para, &para, 0);

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

    else if (argc == 3 && isdigit(*argv[0])) {
        if (!convert_val(argv[2], &value))
            goto usage;

        goto pass;
    }

    else
        goto usage;

pass:
    if (virtual) {
        addr = block = (void *)axtoul(argv[argc - 1]);
        if (!addr) {
            kshell_puts(ctx, "illegal virtual address\n");
            return -ENOMEM;
        }
    } else {
        phys_addr_t phys;

        phys = axtoul(argv[argc - 1]);
        addr = block = ioremap(phys, num * byte);

        if (!addr) {
            kshell_puts(ctx, "failed to remapio\n");
            return -ENOMEM;
        }
    }

    while (num--) {
        uint64_t tmp;

        switch (byte) {
            case 1:
                tmp = readb(addr);
                tmp = do_operator(oper, tmp, value);
                writeb(addr, (uint8_t)tmp);
                break;

            case 2:
                tmp = unaligned_get_u16(addr);
                tmp = do_operator(oper, tmp, value);
                unaligned_set_u16(addr, (uint16_t)tmp);
                break;

            case 4:
                tmp = unaligned_get_u32(addr);
                tmp = do_operator(oper, tmp, value);
                unaligned_set_u32(addr, (uint32_t)tmp);
                break;

            case 8: default:
                tmp = unaligned_get_u64(addr);
                tmp = do_operator(oper, tmp, value);
                unaligned_set_u64(addr, (uint64_t)tmp);
                break;
        }
        addr += byte;
    }

    if (!virtual)
        iounmap(block);

    return -ENOERR;

usage:
    mem_usage(ctx);
    return -EINVAL;
}

#ifdef CONFIG_ARCH_HAS_PMIO
static void ioport_usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: mem -i /nfu <addr> [<operator> <value>]\n");
    kshell_puts(ctx, "\t/n - length\n");
    kshell_puts(ctx, "Format modifiers:\n");
    kshell_puts(ctx, "\t/o - octal\n");
    kshell_puts(ctx, "\t/x - hexadecimal\n");
    kshell_puts(ctx, "\t/d - decimal\n");
    kshell_puts(ctx, "\t/u - unsigned decimal\n");
    kshell_puts(ctx, "Unit modifiers:\n");
    kshell_puts(ctx, "\t/c - char\n");
    kshell_puts(ctx, "\t/b - byte (8-bit)\n");
    kshell_puts(ctx, "\t/h - halfword (16-bit)\n");
    kshell_puts(ctx, "\t/w - word (32-bit)\n");
    operator_value_usage(ctx);
}

static state ioport_in(struct kshell_context *ctx, int argc, char *argv[])
{
    char buff[10], fmt = 'x';
    unsigned int num = 1, byte = 1;
    unsigned int align, count;
    resource_size_t base;

    if (argc == 2 && *argv[0] == '/' && isdigit(*argv[1])) {
        char *para = argv[0] + 1;

        if (!*para)
            goto usage;

        if (isdigit(*para))
            num = strtoui(para, &para, 0);

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
    switch (fmt) {
        case 'o':
            align = oct_align(byte);
            break;

        case 'd':
            align = dec_signed_align(byte);
            break;

        case 'u':
            align = dec_unsigned_align(byte);
            break;

        case 'x':
            align = byte * 2 + 2;
            break;

        default:
            goto usage;
    }

    if (fmt == 'x')
        sprintf(buff, "0x%%0%d%c", align, fmt);
    else
        sprintf(buff, "%%-%d%c", align, fmt);

exit:
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
            kshell_putc(ctx, ' ');
        }
        kshell_putc(ctx, '\n');
    }

    return -ENOERR;

usage:
    ioport_usage(ctx);
    return -EINVAL;
}

static state ioport_out(struct kshell_context *ctx, int argc, char *argv[],
                        enum write_operator oper)
{
    unsigned int num = 1, byte = 1;
    resource_size_t base;
    uint64_t value;

    if (argc == 4 && *argv[0] == '/' && isdigit(*argv[1])) {
        char *para = argv[0] + 1;

        if (!*para || !convert_val(argv[3], &value))
            goto usage;

        if (isdigit(*para))
            num = strtoui(para, &para, 0);

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

    else if (argc == 3 && isdigit(*argv[0])) {
        if (!convert_val(argv[2], &value))
            goto usage;

        goto pass;
    }

    else
        goto usage;

pass:
    base = axtoul(argv[argc - 3]);

    while (num--) {
        uint64_t tmp;

        switch (byte) {
            case 1:
                tmp = inb(base);
                tmp = do_operator(oper, tmp, value);
                outb(base, (uint8_t)tmp);
                break;

            case 2:
                tmp = inw(base);
                tmp = do_operator(oper, tmp, value);
                outw(base, (uint16_t)tmp);
                break;

            case 4: default:
                tmp = inl(base);
                tmp = do_operator(oper, tmp, value);
                outl(base, (uint32_t)tmp);
                break;
        }
        base += byte;
    }

    return -ENOERR;

usage:
    ioport_usage(ctx);
    return -EINVAL;
}
#endif

#ifdef CONFIG_PCI
static void pcicfg_usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: mem -p domain:bus:slot.func /nfu <addr> [<operator> <value>]\n");
    kshell_puts(ctx, "\t/n - length\n");
    kshell_puts(ctx, "Format modifiers:\n");
    kshell_puts(ctx, "\t/o - octal\n");
    kshell_puts(ctx, "\t/x - hexadecimal\n");
    kshell_puts(ctx, "\t/d - decimal\n");
    kshell_puts(ctx, "\t/u - unsigned decimal\n");
    kshell_puts(ctx, "Unit modifiers:\n");
    kshell_puts(ctx, "\t/c - char\n");
    kshell_puts(ctx, "\t/b - byte (8-bit)\n");
    kshell_puts(ctx, "\t/h - halfword (16-bit)\n");
    kshell_puts(ctx, "\t/w - word (32-bit)\n");
    operator_value_usage(ctx);
}

static state pcicfg_read(struct kshell_context *ctx, int argc, char *argv[])
{
    char buff[10], fmt = 'x';
    unsigned int domain, bus, slot, func;
    unsigned int num = 1, byte = 1;
    unsigned int align, count, base;

    if (argc == 3 && *argv[1] == '/' && isdigit(*argv[2])) {
        char *para = argv[1] + 1;

        if (!*para)
            goto usage;

        if (isdigit(*para))
            num = strtoui(para, &para, 0);

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
    switch (fmt) {
        case 'o':
            align = oct_align(byte);
            break;

        case 'd':
            align = dec_signed_align(byte);
            break;

        case 'u':
            align = dec_unsigned_align(byte);
            break;

        case 'x':
            align = byte * 2 + 2;
            break;

        default:
            goto usage;
    }

    if (fmt == 'x')
        sprintf(buff, "0x%%0%d%c", align, fmt);
    else
        sprintf(buff, "%%-%d%c", align, fmt);

exit:
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
            kshell_putc(ctx, ' ');
        }
        kshell_putc(ctx, '\n');
    }

    return -ENOERR;

usage:
    pcicfg_usage(ctx);
    return -EINVAL;
}

static state pcicfg_write(struct kshell_context *ctx, int argc, char *argv[],
                          enum write_operator oper)
{
    unsigned int domain, bus, slot, func;
    unsigned int base;
    int num, byte = 1;
    uint64_t value;

    if (argc == 5 && *argv[1] == '/' && isdigit(*argv[2])) {
        char *para = argv[1] + 1;

        if (!*para || !convert_val(argv[4], &value))
            goto usage;

        if (isdigit(*para))
            num = strtoui(para, &para, 0);

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

    else if (argc == 4 && isdigit(*argv[1])) {
        if (!convert_val(argv[3], &value))
            goto usage;

        goto pass;
    }

    else
        goto usage;

pass:
    if (sscanf(argv[0], "%u:%u:%u.%u", &domain, &bus, &slot, &func) < 0)
        goto usage;
    base = axtou32(argv[argc - 3]);

    while (num--) {
        uint32_t tmp;
        pci_raw_config_read(domain, bus, PCI_DEVFN(slot, func), base, byte, &tmp);
        tmp = do_operator(oper, tmp, value & BIT_LOW_MASK(BITS_PER_BYTE * byte));
        pci_raw_config_write(domain, bus, PCI_DEVFN(slot, func), base, byte, tmp);
        base += byte;
    }

    return -ENOERR;

usage:
    pcicfg_usage(ctx);
    return -EINVAL;
}
#endif

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: mem [option] ...\n");
    kshell_puts(ctx, "\t-r  memory operation (default)\n");
    kshell_puts(ctx, "\t-b  virtual operation\n");
#ifdef CONFIG_ARCH_HAS_PMIO
    kshell_puts(ctx, "\t-o  ioport operation\n");
#endif
#ifdef CONFIG_PCI
    kshell_puts(ctx, "\t-q  pci config operation\n");
#endif
    kshell_puts(ctx, "\t-s  silent output operation\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state mem_main(struct kshell_context *ctx, int argc, char *argv[])
{
    enum mem_action act = ACTION_MEM;
    bool virt = false, silent = false;
    enum write_operator oper;
    state retval;

    if (argc < 2)
        goto usage;

    else if (*argv[1] == '-') {
        char *para = argv[1] + 1;

        if (!*para)
            goto usage;

        for (; *para; para++) {
            switch (*para) {
                case 'm':
                    act = ACTION_MEM;
                    break;

                case 'v':
                    act = ACTION_VIRT;
                    break;

#ifdef CONFIG_ARCH_HAS_PMIO
                case 'i':
                    act = ACTION_IOPORT;
                    break;
#endif

#ifdef CONFIG_PCI
                case 'p':
                    act = ACTION_PCICFG;
                    break;
#endif

                case 's':
                    silent = true;
                    break;

                case 'h': default:
                    goto usage;
            }
        }

        argc--;
        argv++;
    }

    oper = get_operator(argv[argc - 2]);

    switch (act) {
        case ACTION_VIRT:
            virt = true;
            fallthrough;

        case ACTION_MEM: default:
            if (oper != OPERATOR_NULL) {
                retval = mem_write(ctx, argc - 1, &argv[1], oper, virt);
                if (retval || silent)
                    return retval;

                silent = false;
                argc -= 2;
            }

            if (silent)
                goto usage;

            return mem_read(ctx, argc - 1, &argv[1], virt);

#ifdef CONFIG_ARCH_HAS_PMIO
        case ACTION_IOPORT:
            if (oper != OPERATOR_NULL) {
                retval = ioport_out(ctx, argc - 1, &argv[1], oper);
                if (retval || silent)
                    return retval;

                silent = false;
                argc -= 2;
            }

            if (silent)
                goto usage;

            return ioport_in(ctx, argc - 1, &argv[1]);
#endif

#ifdef CONFIG_PCI
        case ACTION_PCICFG:
            if (oper != OPERATOR_NULL) {
                retval = pcicfg_write(ctx, argc - 1, &argv[1], oper);
                if (retval || silent)
                    return retval;

                silent = false;
                argc -= 2;
            }

            if (silent)
                goto usage;

            return pcicfg_read(ctx, argc - 1, &argv[1]);
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
