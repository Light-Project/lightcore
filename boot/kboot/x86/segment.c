/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>
#include <asm/regs.h>
#include <asm/segment.h>

static struct gdt_entry gdt_entry[] = {
#ifdef CONFIG_ARCH_X86_32
    [GDT_ENTRY_BOOT_CS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_4KiB, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
    [GDT_ENTRY_BOOT_DS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_4KiB, .type = GDT_TYPE_RW,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
    [GDT_ENTRY_KERNEL_CS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_4KiB, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
    [GDT_ENTRY_KERNEL_DS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl = GDT_DPL_RING0,
        .db = GDT_DB_32, .g = GDT_G_4KiB, .type = GDT_TYPE_RW,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
#else
    [GDT_LENTRY_KERNEL32_CS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl = GDT_DPL_RING0, .l = false,
        .db = GDT_DB_32, .g = GDT_G_4KiB, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
    [GDT_LENTRY_KERNEL_CS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl = GDT_DPL_RING0, .l = true,
        .db = GDT_DB_16, .g = GDT_G_4KiB, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
    [GDT_LENTRY_KERNEL_DS] = {
        .p = true, .s = GDT_S_CODEDATA, .dpl = GDT_DPL_RING0, .l = true,
        .db = GDT_DB_16, .g = GDT_G_4KiB, .type = GDT_TYPE_XR,
        .basel = 0, .baseh = 0, .limitl = 0xffff, .limith = 0xf,
    },
#endif
};

struct gdt_table gdt_table = {
    .limit = sizeof(gdt_entry) - 1,
};

void segment_init(void)
{
    gdt_table.gdt = gdt_entry;
    gdt_set(&gdt_table);
}
