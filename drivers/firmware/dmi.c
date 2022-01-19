/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(count) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"DMI: " fmt

#include <ctype.h>
#include <string.h>
#include <kmalloc.h>
#include <ioremap.h>
#include <printk.h>
#include <panic.h>
#include <driver/firmware/dmi.h>
#include <lightcore/asm/byteorder.h>

#define DMI_ENTRY_POINT_START   0xf0000
#define DMI_ENTRY_POINT_SIZE    0x10000

static void *dmi_entry;
static uint32_t dmi_version;
static uint32_t dmi_length;
static uint16_t dmi_number;

static unsigned int __init print_filtered(char *buf, size_t len, const char *info)
{
    unsigned int count = 0;
    const char *str;

    if (!info)
        return 0;

    for (str = info; *str; str++) {
        if (isprint(*str))
            count += scnprintf(buf + count, len - count, "%c", *str);
        else
            count += scnprintf(buf + count, len - count, "\\x%02x", *str & 0xff);
    }

    return count;
}

static void __init dmi_board_name(char *buf, size_t len)
{
    unsigned int count = 0;
    const char *board;

    count += print_filtered(buf + count, len - count, dmi_get_ident(DMI_REG_SYS_VENDOR));
    count += scnprintf(buf + count, len - count, " ");
    count += print_filtered(buf + count, len - count, dmi_get_ident(DMI_REG_PRODUCT_NAME));

    board = dmi_get_ident(DMI_REG_BOARD_NAME);
    if (board) {
        count += scnprintf(buf + count, len - count, "/");
        count += print_filtered(buf + count, len - count, board);
    }

    count += scnprintf(buf + count, len - count, ", BIOS ");
    count += print_filtered(buf + count, len - count, dmi_get_ident(DMI_REG_BIOS_VERSION));
    count += scnprintf(buf + count, len - count, " ");
    count += print_filtered(buf + count, len - count, dmi_get_ident(DMI_REG_BIOS_DATE));
}

/**
 * dmi_decode -
 *
 */
static void __init dmi_decode(const struct dmi_header *header)
{
    switch (header->type) {
        case DMI_ENTRY_BIOS:
            dmi_save_ident(header, DMI_REG_BIOS_VENDOR, DMI_HD_BIOS_VENDOR);
            dmi_save_ident(header, DMI_REG_BIOS_VERSION, DMI_HD_BIOS_VERSION);
            dmi_save_ident(header, DMI_REG_BIOS_DATE, DMI_HD_BIOS_DATE);
            dmi_save_release(header, DMI_REG_BIOS_RELEASE, DMI_HD_BIOS_MINOR);
            dmi_save_release(header, DMI_REG_EC_RELEASE, DMI_HD_EC_MINOR);
            break;
        case DMI_ENTRY_SYSTEM:
            dmi_save_ident(header, DMI_REG_SYS_VENDOR, DMI_HD_SYS_MANUFACT);
            dmi_save_ident(header, DMI_REG_PRODUCT_NAME, DMI_HD_SYS_PRODUCT);
            dmi_save_ident(header, DMI_REG_PRODUCT_VERSION, DMI_HD_SYS_VERSION);
            dmi_save_ident(header, DMI_REG_PRODUCT_SERIAL, DMI_HD_SYS_SERIAL);
            dmi_save_ident(header, DMI_REG_PRODUCT_SKU, DMI_HD_SYS_SKU);
            dmi_save_ident(header, DMI_REG_PRODUCT_FAMILY, DMI_HD_SYS_FAMILY);
            break;
        case DMI_ENTRY_BASEBOARD:
            break;
        case DMI_ENTRY_CHASSIS:
            break;
        case DMI_ENTRY_OEMSTRINGS:
            break;
    }
}

/**
 * dmi_decode_all -
 *
 */
static void __init dmi_decode_all(void)
{
    const void *entry = dmi_entry;
    uint16_t count = 0;

    /*
     * Stop when we have seen all the items the table claimed to have
     * (SMBIOS < 3.0 only) OR we reach an end-of-table marker (SMBIOS
     * >= 3.0 only) OR we run off the end of the table (should never
     * happen but sometimes does on bogus implementations.)
     */

    while ((!dmi_number || count++ < dmi_number) &&
           (entry - dmi_entry + sizeof(struct dmi_header)) <= dmi_length) {
        const struct dmi_header *header = entry;

        /*
         *  We want to know the total length (formatted area and
         *  strings) before decoding to make sure we won't run off the
         *  table in dmi_decode or dmi_string
         */

        entry += header->length;
        while ((entry - dmi_entry < dmi_length - 1) &&
               (((uint8_t *)entry)[0] || ((uint8_t *)entry)[1]))
            entry++;

        if (entry - dmi_entry < dmi_length - 1)
            dmi_decode(header);

        entry += 2;
        count += 1;

        /*
         * 7.45 End-of-Table (Type 127) [SMBIOS reference spec v3.0.0]
         * For tables behind a 64-bit entry point, we have no item
         * count and no exact table length, so stop on end-of-table
         * marker. For tables behind a 32-bit entry point, we have
         * seen OEM structures behind the end-of-table marker on
         * some systems, so don't trust it.
         */

        if (header->type == DMI_ENTRY_END_OF_TABLE)
            break;
    }
}

/**
 * dmi_checksum -
 *
 */
static bool __init dmi_checksum(const void *addr, uint8_t len)
{
    const uint8_t *block = addr;
    uint8_t sum = 0;

    while (len--)
        sum += block[len];

    return !sum;
}

/**
 * dmi3_head_check -
 *
 */
static bool __init dmi3_head_check(const struct dmi3_entry_point *entry)
{
    phys_addr_t phys_entry;

    if (memcmp(entry->anchor, SM_ANCHOR, 4) || entry->length > 32 ||
        !dmi_checksum(entry, entry->length))
        return false;

    dmi_version = entry->docrev << 16 | entry->major << 8 | entry->major;
    dmi_length = le32_to_cpu(entry->strmax);
    phys_entry = entry->straddr;
    dmi_entry = ioremap_uc(phys_entry, dmi_length);

    return true;
}

/**
 * dmi_head_check -
 *
 */
static bool __init dmi_head_check(const struct dmi_entry_point *entry)
{
    phys_addr_t phys_entry;
    uint16_t smbios = 0;

    if (memcmp(entry->inanchor, DMI_ANCHOR, 5) ||
        !dmi_checksum(entry->inanchor, 15))
        return false;

    if (!memcmp(entry->anchor, SM_ANCHOR, 4) &&
        entry->length < 32 && dmi_checksum(entry, entry->length)) {
        smbios = (entry->major << 8) | entry->minor;

        /* Fixup smbios version */
        switch (smbios) {
            case 0x021f:
                fallthrough;
            case 0x0221:
                pr_info("SMBIOS version fixup (2.%d->2.%d)\n", smbios & 0xFF, 3);
                smbios = 0x0203;
                break;
            case 0x0233:
                pr_info("SMBIOS version fixup (2.%d->2.%d)\n", 51, 6);
                smbios = 0x0206;
                break;
        }
    }

    if (smbios) {
        dmi_version = smbios << 8;
        pr_info("SMBIOS %d.%d present.\n",
            dmi_version >> 16, (dmi_version >> 8) & 0xff);
    } else {
        dmi_version = ((entry->bcdrev & 0xf0) << 4 | (entry->bcdrev & 0x0F)) << 8;
        pr_info("Legacy DMI %d.%d present.\n",
            dmi_version >> 16, (dmi_version >> 8) & 0xff);
    }

    dmi_number = le16_to_cpu(entry->strnr);
    dmi_length = le16_to_cpu(entry->strlen);
    phys_entry = le32_to_cpu(entry->straddr);
    dmi_entry = ioremap_uc(phys_entry, dmi_length);

    return true;
}

void __init dmi_init(void)
{
    const void *entry, *walk;
    char board[128];

    entry = ioremap_uc(DMI_ENTRY_POINT_START, DMI_ENTRY_POINT_SIZE);
    if (!entry)
        panic("DMI: Failed to allocate\n");

    for (walk = entry; walk < entry + DMI_ENTRY_POINT_SIZE; walk += 16) {
        if (dmi3_head_check(walk) || dmi_head_check(walk))
            break;
    } if (!dmi_entry) {
        pr_warn("entry not found\n");
        return;
    }

    dmi_decode_all();
    dmi_board_name(board, sizeof(board));
    pr_info("%s\n", board);
}
