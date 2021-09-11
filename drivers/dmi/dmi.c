/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"DMI: " fmt

#include <string.h>
#include <mm.h>
#include <initcall.h>
#include <driver/dmi.h>
#include <printk.h>

#define DMI_ENTRY_POINT_START   0xf0000
#define DMI_ENTRY_POINT_END     0x100000

static void *dmi_start_addr;
static uint32_t dmi_length;
static uint16_t dmi_number;

static void dmi_decode(struct dmi_header *header)
{
    switch (header->type) {
        case DMI_ENTRY_BIOS:
            dmi_region_string(header, DMI_HD_BIOS_VENDOR, DMI_REG_BIOS_VENDOR);
            dmi_region_string(header, DMI_HD_BIOS_VERSION, DMI_REG_BIOS_VERSION);
            dmi_region_string(header, DMI_HD_BIOS_DATE, DMI_REG_BIOS_DATE);
            dmi_region_word(header, DMI_HD_BIOS_MINOR, DMI_REG_BIOS_RELEASE);
            dmi_region_word(header, DMI_HD_EC_MINOR, DMI_REG_EC_RELEASE);
            break;
        case DMI_ENTRY_SYSTEM:
            dmi_region_string(header, DMI_REG_SYS_VENDOR, DMI_HD_SYS_MANUFACT);
            dmi_region_string(header, DMI_REG_PRODUCT_NAME, DMI_HD_SYS_PRODUCT);
            dmi_region_string(header, DMI_REG_PRODUCT_VERSION, DMI_HD_SYS_VERSION);
            dmi_region_string(header, DMI_REG_PRODUCT_SERIAL, DMI_HD_SYS_SERIAL);
            dmi_region_string(header, DMI_REG_PRODUCT_SKU, DMI_HD_SYS_SKU);
            dmi_region_string(header, DMI_REG_PRODUCT_FAMILY, DMI_HD_SYS_FAMILY);
            break;
        case DMI_ENTRY_BASEBOARD:
            break;
        case DMI_ENTRY_CHASSIS:
            break;
        case DMI_ENTRY_OEMSTRINGS:
            break;
    }
}

state dmi_decode_all(void)
{
    struct dmi_header *header;
    void *entry;
    uint16_t count;

    for (entry = dmi_start_addr;
         dmi_length < count++ &&
         entry - dmi_start_addr < dmi_length;
         entry += header->length) {
        header = entry;

        if (header->type == DMI_ENTRY_END_OF_TABLE)
            break;

        dmi_decode(header);
    }

    return -ENOERR;
}

static bool dmi_checksum(void *addr, uint8_t len)
{
    uint8_t *block = addr;
    uint8_t sum = 0;
    while (len--)
        sum += block[len];
    return sum == 0;
}

static __init bool dmi_head_check(struct dmi_entry_point *entry)
{
    uint16_t version = 0;

    if (memcmp(entry->anchor,   SM_ANCHOR,  4) ||
        memcmp(entry->inanchor, DMI_ANCHOR, 5) ||
        !dmi_checksum(entry, entry->length) ||
        !dmi_checksum(entry->inanchor, 15)  ||
        entry->length > 32)
        return false;

    version = (entry->major << 8) | entry->minor;

    /* Fixup smbios quirks */
    switch (version) {
        case 0x021f:
            fallthrough;
        case 0x0221:
            version = 0x0203;
            break;
        case 0x0233:
            version = 0x0206;
            break;
    }

    dmi_number = entry->strnr;
    dmi_length = entry->strlen;
    dmi_start_addr = ioremap(entry->straddr, dmi_length);
    return true;
}

static __init bool dmi3_head_check(struct dmi3_entry_point *entry)
{
    if (memcmp(entry->anchor, SM_ANCHOR, 4) ||
        entry->length > 32 || !dmi_checksum(entry, entry->length))
        return false;
    return true;
}

static void dmi_scan(void)
{
    phys_addr_t addr;
    void *entry;

    for (addr = DMI_ENTRY_POINT_START;
         addr < DMI_ENTRY_POINT_END; addr++) {
        entry = ioremap(addr, 32);
        if (dmi3_head_check(entry) ||
            dmi_head_check(entry))
            break;
        iounmap(entry);
    } if (!dmi_start_addr) {
        pr_warn("entry not found.");
        return;
    }

    dmi_decode_all();
}

static state __init dmi_init(void)
{
    dmi_scan();
    if (!dmi_start_addr)
        return -ENODATA;

    return -ENOERR;
}
arch_initcall(dmi_init);
