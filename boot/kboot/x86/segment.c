/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>
#include <asm/regs.h>
#include <asm/segment.h>

#define GDT_SET(Table, Num, Base, Limit, Type,      \
                S, Dpl, Db , G)                     \
do {                                                \
    Table[Num].basel   = (Base) & 0xffffff;         \
    Table[Num].baseh   = ((Base) >> 24) & 0xff;     \
    Table[Num].limitl  = ((Limit) & 0xffff);        \
    Table[Num].limith  = ((Limit) >> 16) & 0xf;     \
    Table[Num].type    = (Type) & 0xff;             \
    Table[Num].s       = (S) & 0x01;                \
    Table[Num].dpl     = (Dpl) & 0x03;              \
    Table[Num].p       = 0x01;                      \
    Table[Num].db      = (Db) & 0x01;               \
    Table[Num].g       = (G) & 0x01;                \
} while(0)

#define GDT_SET_DEFAULTS_BOOTLOADER_CS(GDT_ENTRY)   \
    GDT_SET(GDT_ENTRY, GDT_ENTRY_BOOT_CS,           \
    0, GDT_LIMIT_G_4GiB, GDT_TYPE_XRA,              \
    GDT_S_CODEDATA, GDT_DPL_RING0, GDT_DB_32, GDT_G_4KiB)

#define GDT_SET_DEFAULTS_BOOTLOADER_DS(GDT_ENTRY)   \
    GDT_SET(GDT_ENTRY, GDT_ENTRY_BOOT_DS,           \
    0, GDT_LIMIT_G_4GiB, GDT_TYPE_RWA,              \
    GDT_S_CODEDATA, GDT_DPL_RING0, GDT_DB_32, GDT_G_4KiB)

#define GDT_SET_DEFAULTS_KERNEL_CS(GDT_ENTRY)       \
    GDT_SET(GDT_ENTRY, GDT_ENTRY_KERNEL_CS,         \
    0, GDT_LIMIT_G_4GiB, GDT_TYPE_XRA,              \
    GDT_S_CODEDATA, GDT_DPL_RING0, GDT_DB_32, GDT_G_4KiB)

#define GDT_SET_DEFAULTS_KERNEL_DS(GDT_ENTRY)       \
    GDT_SET(GDT_ENTRY, GDT_ENTRY_KERNEL_DS,         \
    0, GDT_LIMIT_G_4GiB, GDT_TYPE_RWA,              \
    GDT_S_CODEDATA, GDT_DPL_RING0, GDT_DB_32, GDT_G_4KiB)

struct gdt_entry gdt_entry[GDT_ENTRY_MAX] __aligned(0x10);
struct gdt_table gdt_table;

void segment_init(void)
{
    gdt_table.limit = sizeof(gdt_entry) - 1;
    gdt_table.gdt = gdt_entry;

    GDT_SET_DEFAULTS_BOOTLOADER_CS(gdt_entry);
    GDT_SET_DEFAULTS_BOOTLOADER_DS(gdt_entry);
    GDT_SET_DEFAULTS_KERNEL_CS(gdt_entry);
    GDT_SET_DEFAULTS_KERNEL_DS(gdt_entry);

    gdt_set(&gdt_table);
}
