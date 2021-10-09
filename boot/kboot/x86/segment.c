/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>
#include <asm/gdt.h>

#define GDT_SET(table, num, base, limit,            \
                _type, _s, _dpl, _db , _g)          \
        do {                                        \
        table[num].basel   = (base) & 0xffffff;     \
        table[num].baseh   = ((base) >> 24) & 0xff; \
        table[num].limitl  = ((limit) & 0xffff);    \
        table[num].limith  = ((limit) >> 16) & 0xf; \
        table[num].type    = (_type) & 0xff;        \
        table[num].s       = (_s) & 0x01;           \
        table[num].dpl     = (_dpl) & 0x03;         \
        table[num].p       = 0x01;                  \
        table[num].db      = (_db) & 0x01;          \
        table[num].g       = (_g) & 0x01;           \
        } while(0)           

#define GDT_SET_DEFAULTS_BOOTLOADER_CS(GDT_ENTRY)   \
        GDT_SET(GDT_ENTRY, GDT_ENTRY_BOOT_CS,       \
        0, GDT_LIMIT_G_4GiB, GDT_TYPE_XRA,          \
        GDT_S_CODEDATA, GDT_DPL_RING0, GDT_DB_32, GDT_G_4KiB)

#define GDT_SET_DEFAULTS_BOOTLOADER_DS(GDT_ENTRY)   \
        GDT_SET(GDT_ENTRY, GDT_ENTRY_BOOT_DS,       \
        0, GDT_LIMIT_G_4GiB, GDT_TYPE_RWA,          \
        GDT_S_CODEDATA, GDT_DPL_RING0, GDT_DB_32, GDT_G_4KiB)

#define GDT_SET_DEFAULTS_KERNEL_CS(GDT_ENTRY)       \
        GDT_SET(GDT_ENTRY, GDT_ENTRY_KERNEL_CS,     \
        0, GDT_LIMIT_G_4GiB, GDT_TYPE_XRA,          \
        GDT_S_CODEDATA, GDT_DPL_RING0, GDT_DB_32, GDT_G_4KiB)

#define GDT_SET_DEFAULTS_KERNEL_DS(GDT_ENTRY)       \
        GDT_SET(GDT_ENTRY, GDT_ENTRY_KERNEL_DS,     \
        0, GDT_LIMIT_G_4GiB, GDT_TYPE_RWA,          \
        GDT_S_CODEDATA, GDT_DPL_RING0, GDT_DB_32, GDT_G_4KiB)

struct gdt_entry GDT_ENTRY[GDT_ENTRY_MAX] __attribute__((aligned(0x10)));
struct gdt_table gdt_table;

static inline void gdt_load(void *addr)
{
    asm volatile("lgdtl (%0)"::"Na"(addr));
}

void segment_init(void)
{
    gdt_table.limit = sizeof(GDT_ENTRY);
    gdt_table.gdt = GDT_ENTRY;

    GDT_SET_DEFAULTS_BOOTLOADER_CS(GDT_ENTRY);
    GDT_SET_DEFAULTS_BOOTLOADER_DS(GDT_ENTRY);

    GDT_SET_DEFAULTS_KERNEL_CS(GDT_ENTRY);
    GDT_SET_DEFAULTS_KERNEL_DS(GDT_ENTRY);

    gdt_load(&gdt_table);
}
