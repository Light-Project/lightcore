/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <mm/vmem.h>
#include <size.h>

#include <asm/page.h>
#include <asm/gdt.h>

static struct gdt_entry gdt_entry[GDT_ENTRY_MAX] = {
    [GDT_ENTRY_KERNEL_CS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_4KiB, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
    [GDT_ENTRY_KERNEL_DS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_4KiB, .type = GDT_TYPE_RW,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
    [GDT_ENTRY_USER_CS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING3,
        .db = GDT_DB_32, .g = GDT_G_4KiB, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
    [GDT_ENTRY_USER_DS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING3,
        .db = GDT_DB_32, .g = GDT_G_4KiB, .type = GDT_TYPE_RW,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
    [GDT_ENTRY_PNPBIOS_CS32] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_1BYTE, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0x0,
    },
    [GDT_ENTRY_PNPBIOS_CS16] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_16, .g = GDT_G_1BYTE, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0x0,
    },
    [GDT_ENTRY_PNPBIOS_DS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_16, .g = GDT_G_1BYTE, .type = GDT_TYPE_RW,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0x0,
    },
    [GDT_ENTRY_PNPBIOS_TS1] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_16, .g = GDT_G_1BYTE, .type = GDT_TYPE_RW,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0x0,
    },
    [GDT_ENTRY_PNPBIOS_TS2] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_16, .g = GDT_G_1BYTE, .type = GDT_TYPE_RW,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0x0,
    },
    [GDT_ENTRY_APMBIOS_CS32] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_1BYTE, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0x0,
    },
    [GDT_ENTRY_APMBIOS_CS16] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_16, .g = GDT_G_1BYTE, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0x0,
    },
    [GDT_ENTRY_APMBIOS_DS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_1BYTE, .type = GDT_TYPE_RW,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0x0,
    },
    [GDT_ENTRY_TSS] = {
        .p = true, .s = GDT_S_SYSTEM, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_1BYTE, .type = GDT_TYPE_XA,
        .basel = 0, .baseh = 0, .limitl = 0, .limith = 0x0,
    },
    [GDT_ENTRY_LDT] = {
        .p = true, .s = GDT_S_SYSTEM, .dpl  = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_1BYTE, .type = GDT_TYPE_XA,
        .basel = 0, .baseh = 0, .limitl = 0, .limith = 0x0,
    },
};

struct gdt_table gdt_table = {
    .gdt = gdt_entry,
    .limit = sizeof(gdt_entry),
};

void gdte_seg_set(int index, char dpl, char type, size_t base, uint64_t limit)
{
    limit >>= PAGE_SHIFT;
    gdt_entry[index].basel  = base;
    gdt_entry[index].baseh  = base >> 24;
    gdt_entry[index].limitl = limit;
    gdt_entry[index].limith = limit >> 16;
    gdt_entry[index].type   = type;
    gdt_entry[index].dpl    = dpl;
    gdt_entry[index].p      = true;
    gdt_entry[index].s      = GDT_S_CODEDATA;
    gdt_entry[index].db     = GDT_DB_32;
    gdt_entry[index].g      = GDT_G_4KiB;

#ifdef CONFIG_ARCH_X86_64
    gdt_entry[index].basee  = base >> 32;
#endif
}

void gdte_sys_set(int index, char type, size_t base, uint64_t limit)
{
    gdt_entry[index].basel  = base;
    gdt_entry[index].baseh  = base >> 24;
    gdt_entry[index].limitl = limit;
    gdt_entry[index].limith = limit >> 16;
    gdt_entry[index].type   = type;
    gdt_entry[index].dpl    = GDT_DPL_RING0;
    gdt_entry[index].p      = true;
    gdt_entry[index].s      = GDT_S_SYSTEM;
    gdt_entry[index].db     = GDT_DB_16;
    gdt_entry[index].g      = GDT_G_1BYTE;

#ifdef CONFIG_ARCH_X86_64
    gdt_entry[index].basee  = base >> 32;
#endif
}

void __init gdt_setup(void)
{
    gdt_set(&gdt_table);
}
