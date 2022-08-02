/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>
#include <arch/x86/segment.h>
#include <arch/x86/interrupt.h>

static struct gdt_entry gdt32_entry[] = {
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
};

struct gdt_table gdt32_table = {
    .gdt = gdt32_entry,
    .limit = sizeof(gdt32_entry) - 1,
};

struct idt_table idt32_table = {
    .idt = 0,
    .limit = 0,
};

#ifdef CONFIG_ARCH_X86_64
static struct gdt_entry gdt64_entry[] = {
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
};

struct gdt_table gdt64_table = {
    .gdt = gdt64_entry,
    .limit = sizeof(gdt64_entry) - 1,
};
#endif /* CONFIG_ARCH_X86_64 */
