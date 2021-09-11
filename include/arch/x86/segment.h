/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_SEGMENT_H_
#define _ARCH_X86_SEGMENT_H_

#include <types.h>
#include <arch/x86/seg.h>

struct gdt_entry{
    uint64_t limitl:16;     // Segment limit low
    uint64_t basel:24;      // Segment Base address low
    uint64_t type:4;        // Segment access rights (x r w a)
    uint64_t s:1;           // Decriptor type (system / segment)
    uint64_t dpl:2;         // Decriptor privilege level (ring 0-3)
    uint64_t p:1;           // Segment present (0:non-existent 1:In memory)
    uint64_t limith:4;      // Segment limit high
    uint64_t avl:1;         // Available for use by system software (hardware retention)
    uint64_t RES:1;         // Must write 0
    uint64_t db:1;          // Defaults type (0:16bit 1:32bit)
    uint64_t g:1;           // Granularity (0:1byte 1:4KiB)
    uint64_t baseh:8;       // Segment Base address high
} __packed;

struct gdt_table{
    uint16_t limit;         // GDT_ENTRY limit (byte)
    struct gdt_entry *gdt;  // Point to GDT_ENTRY
} __packed;

#define GDT_LIMIT_G_4KiB    0x00001     //
#define GDT_LIMIT_G_4MiB    0x00400     //
#define GDT_LIMIT_G_4GiB    0xFFFFF     //

#define GDT_TYPE_RO     0x00            //
#define GDT_TYPE_RA     0x01            //
#define GDT_TYPE_RW     0x02            //
#define GDT_TYPE_RWA    0x03            //
#define GDT_TYPE_RE     0x04            //
#define GDT_TYPE_REA    0x05            //
#define GDT_TYPE_RWE    0x06            //
#define GDT_TYPE_RWEA   0x07            //
#define GDT_TYPE_XO     0x08            //
#define GDT_TYPE_XA     0x09            //
#define GDT_TYPE_XR     0x0A            //
#define GDT_TYPE_XRA    0x0B            //
#define GDT_TYPE_XC     0x0C            //
#define GDT_TYPE_XCA    0x0D            //
#define GDT_TYPE_XRC    0x0E            //
#define GDT_TYPE_XRCA   0x0F            //

#define GDT_S_SYSTEM    0x00            //
#define GDT_S_CODEDATA  0x01            //

#define GDT_DPL_RING0   0X00            //
#define GDT_DPL_RING1   0X01            //
#define GDT_DPL_RING2   0X02            //
#define GDT_DPL_RING3   0X03            //

#define GDT_DB_16       0x00            //
#define GDT_DB_32       0x01            //

#define GDT_G_1BYTE     0x00            //
#define GDT_G_4KiB      0x01            //

#endif
