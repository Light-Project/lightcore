/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_SEGMENT_H_
#define _ARCH_X86_SEGMENT_H_

#include <types.h>
#include <arch/x86/seg.h>

#ifndef __ASSEMBLY__

struct gdt_entry {
    uint64_t limitl:16;     /* Segment limit low */
    uint64_t basel:24;      /* Segment Base address low */
    uint64_t type:4;        /* Segment access rights (x r w a) */
    uint64_t s:1;           /* Decriptor type (system / segment) */
    uint64_t dpl:2;         /* Decriptor privilege level (ring 0-3) */
    uint64_t p:1;           /* Segment present (0:non-existent 1:In memory) */
    uint64_t limith:4;      /* Segment limit high */
    uint64_t avl:1;         /* Available for use by system software (hardware retention) */
    uint64_t l:1;           /* 64-bit code segment (IA-32e mode only) */
    uint64_t db:1;          /* Defaults type (0:16bit 1:32bit) */
    uint64_t g:1;           /* Granularity (0:1byte 1:4KiB) */
    uint64_t baseh:8;       /* Segment Base address high */
} __packed;

struct gdt_table {
    uint16_t limit;         /* GDT_ENTRY limit (byte) */
    struct gdt_entry *gdt;  /* Point to GDT_ENTRY */
} __packed;

#endif  /* __ASSEMBLY__ */

#define GDT_LIMIT_G_4KiB    0x00001 /* 4KiB limit segment */
#define GDT_LIMIT_G_4MiB    0x00400 /* 4MiB limit segment */
#define GDT_LIMIT_G_4GiB    0xfffff /* 4GiB limit segment */

/* Code-Segment and Data-Segment Types */
#define GDT_TYPE_RO         0x00    /* Read-Only */
#define GDT_TYPE_RA         0x01    /* Read-Only, accessed */
#define GDT_TYPE_RW         0x02    /* Read/Write */
#define GDT_TYPE_RWA        0x03    /* Read/Write, accessed */
#define GDT_TYPE_RE         0x04    /* Read-Only, expand-down */
#define GDT_TYPE_REA        0x05    /* Read-Only, expand-down, accessed */
#define GDT_TYPE_RWE        0x06    /* Read/Write, expand-down */
#define GDT_TYPE_RWEA       0x07    /* Read/Write, expand-down, accessed */
#define GDT_TYPE_XO         0x08    /* Execute-Only */
#define GDT_TYPE_XA         0x09    /* Execute-Only, accessed */
#define GDT_TYPE_XR         0x0a    /* Execute/Read */
#define GDT_TYPE_XRA        0x0b    /* Execute/Read, accessed */
#define GDT_TYPE_XC         0x0c    /* Execute-Only, conforming */
#define GDT_TYPE_XCA        0x0d    /* Execute-Only, conforming, accessed */
#define GDT_TYPE_XRC        0x0e    /* Execute/Read, conforming */
#define GDT_TYPE_XRCA       0x0f    /* Execute/Read, conforming, accessed */

/* System-Segment and Gate-Descriptor Types */
#define GDT_TYPE_RES0       0x00    /* Reserved */
#define GDT_TYPE_TSS_16A    0x01    /* 16-bit TSS (Available) */
#define GDT_TYPE_LDT        0x02    /* LDT */
#define GDT_TYPE_TSS_16B    0x03    /* 16-bit TSS (Busy) */
#define GDT_TYPE_TASK_16    0x04    /* 16-bit Call Gate */
#define GDT_TYPE_TASK       0x05    /* Task Gate */
#define GDT_TYPE_INT_16     0x06    /* 16-bit Interrupt Gate */
#define GDT_TYPE_TRAP_16    0x07    /* 16-bit Trap Gate */
#define GDT_TYPE_RES1       0x08    /* Reserved */
#define GDT_TYPE_TSSA       0x09    /* 32-bit TSS (Available) */
#define GDT_TYPE_RES2       0x0a    /* Reserved */
#define GDT_TYPE_TSSB       0x0b    /* 32-bit TSS (Busy) */
#define GDT_TYPE_CALL       0x0c    /* 32-bit Call Gate */
#define GDT_TYPE_RES3       0x0d    /* Reserved */
#define GDT_TYPE_INT        0x0e    /* 32-bit Interrupt Gate */
#define GDT_TYPE_TARP       0x0f    /* 32-bit Trap Gate */

#define GDT_S_SYSTEM        0x00    /* descriptor type is a system descriptor */
#define GDT_S_CODEDATA      0x01    /* descriptor is for either a code or a data segment */

#define GDT_DPL_RING0       0x00    /* Descriptor privilege level0 */
#define GDT_DPL_RING1       0x01    /* Descriptor privilege level1 */
#define GDT_DPL_RING2       0x02    /* Descriptor privilege level2 */
#define GDT_DPL_RING3       0x03    /* Descriptor privilege level3 */

#define GDT_DB_16           0x00    /* 16-bit code and data segments */
#define GDT_DB_32           0x01    /* 32-bit code and data segments */

#define GDT_G_1BYTE         0x00    /* Segments granularity 1Byte */
#define GDT_G_4KiB          0x01    /* Segments granularity 1KiB */

#endif  /* _ARCH_X86_SEGMENT_H_ */
