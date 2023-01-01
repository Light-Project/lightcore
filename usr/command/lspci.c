/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <kernel.h>
#include <initcall.h>
#include <driver/pci.h>
#include <kshell.h>
#include <printk.h>

#define LSPCI_FLAG_STAND BIT(0)
#define LSPCI_FLAG_WHOLE BIT(1)
#define LSPCI_FLAG_EXTEN BIT(2)

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: lspci [option] bus:slot.func\n");
    kshell_printf(ctx, "\t-x  show the standard part of the config space\n");
    kshell_printf(ctx, "\t-a  show the whole config space\n");
    kshell_printf(ctx, "\t-e  show the 4096-byte extended config space\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static void lspci_device_show(struct kshell_context *ctx, unsigned int bus, unsigned int devfn,
                              uint16_t class, uint16_t vendor, uint16_t device)
{
    kshell_printf(ctx, "%02u:%02u.%u Class %#06x: %#06x:%#06x\n",
        bus, PCI_SLOT(devfn), PCI_FUNC(devfn), class, vendor, device);
}

static void lspci_device_dump(struct kshell_context *ctx, uint8_t flags, unsigned int bus, unsigned int devfn)
{
    unsigned int size, rowc, offset = 0;
    uint32_t value;

    if (flags & LSPCI_FLAG_STAND)
        size = PCI_STD_HEADER_SIZEOF;
    else if (flags & LSPCI_FLAG_WHOLE)
        size = PCI_CFG_SPACE_SIZE;
    else if (flags & LSPCI_FLAG_EXTEN)
        size = PCI_CFG_SPACE_EXP_SIZE;
    else
        return;

    while (offset < size) {
        kshell_printf(ctx, "%04x:", offset);
        for (rowc = 0; rowc < 0x10; ++rowc) {
            pci_raw_config_read(0, bus, devfn, offset++, 1, &value);
            kshell_printf(ctx, " %02x", value);
        }
        kshell_putc(ctx, '\n');
    }
    kshell_putc(ctx, '\n');
}

static bool lspci_scan_device(struct kshell_context *ctx, uint8_t flags, unsigned int bus, unsigned int devfn)
{
    uint16_t vendor, device;
    uint32_t value, class;

    if (pci_raw_config_read(0, bus, devfn, PCI_VENDOR_ID, sizeof(value), &value) ||
        pci_raw_config_read(0, bus, devfn, PCI_CLASS_REVISION, sizeof(class), &class))
        return false;

    if (value == 0xffffffff || value == 0x00000000 ||
        value == 0x0000ffff || value == 0xffff0000)
        return true;

    vendor = value & 0xffff;
    device = (value >> 16) & 0xffff;
    class >>= 8;

    lspci_device_show(ctx, bus, devfn, class, vendor, device);
    lspci_device_dump(ctx, flags, bus, devfn);

    return true;
}

static void lspci_scan_machine(struct kshell_context *ctx, uint8_t flags)
{
    unsigned int bus, devfn;

    for (bus = 0;; ++bus) {
        for (devfn = 0; devfn < PCI_MAX_SLOT * PCI_MAX_FUNCTION; ++devfn) {
            if (!lspci_scan_device(ctx, flags, bus, devfn))
                return;
        }
    }
}

static state lspci_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int bus, slot, func;
    unsigned int count;
    uint8_t flags = 0;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'x':
                flags = LSPCI_FLAG_STAND;
                break;

            case 'a':
                flags = LSPCI_FLAG_WHOLE;
                break;

            case 'e':
                flags = LSPCI_FLAG_EXTEN;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (count == argc) {
        lspci_scan_machine(ctx, flags);
        return -ENOERR;
    }

    sscanf(argv[count], "%u:%u.%u", &bus, &slot, &func);
    lspci_scan_device(ctx, flags, bus, PCI_DEVFN(slot, func));

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command lspci_cmd = {
    .name = "lspci",
    .desc = "list all pci devices",
    .exec = lspci_main,
};

static state lspci_init(void)
{
    return kshell_register(&lspci_cmd);
}
kshell_initcall(lspci_init);
