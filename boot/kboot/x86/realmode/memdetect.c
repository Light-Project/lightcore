/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot16.h>
#include <asm/regs.h>

#define SMAP 0x534d4150
struct bootparam bootparam __section(".startup");

static void reg_init(struct bios_reg *reg)
{
    reg->eflags = EFLAGS_CF;
    reg->ds = ds_get();
    reg->es = ds_get();
}

void memdetect(void)
{
    struct bios_reg oreg, ireg = {};
    struct e820_entry buf;
    unsigned int count = 0;

    reg_init(&ireg);
    ireg.ax  = 0xe820;
    ireg.cx  = sizeof(buf);
    ireg.edx = SMAP;
    ireg.di  = (size_t)&buf;

    do {
        bios_int(0x15, &ireg, &oreg);
        ireg.ebx = oreg.ebx;

        if (oreg.eflags & EFLAGS_CF)
            break;

        if (oreg.eax != SMAP) {
            count = 0;
            break;
        }

        bootparam.e820_table.entry[count++] = buf;
    } while (ireg.ebx && count < E820_MAX_ENTRIES);

    bootparam.e820_table.entry_nr = count;
}
