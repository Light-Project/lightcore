/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ARCH_ARM_PAGE_H_
#define _ARCH_ARM_PAGE_H_

#include <types.h>

#ifndef __ASSEMBLY__

struct pgd {
    union {
        struct {
            uint32_t type:2;    /* Page type (1: Coarse 2:Section 3:Fine) */
            uint32_t b:1;       /* Page-level write-through */
            uint32_t c:1;       /* Page-level cache enable */
            uint32_t p:1;       /* Must write 1 */
            uint32_t dm:4;      /* Domain control bits */
        };
        struct {                /* Fine page table */
            uint32_t res:12;
            uint32_t fine:20;
        };
        struct {                /* Coarse page table */
            uint32_t _res:10;
            uint32_t coarse:22;
        };
        struct {                /* Section */
            uint32_t __res:10;
            uint32_t ap:2;
            uint32_t __res1:8;
            uint32_t section:12;
        };
        uint32_t val;
    };
} __packed;

struct pte {
    union {
        struct {
            uint32_t type:2;
            uint32_t b:1;
            uint32_t c:1;
            uint32_t ap:2;
        };
        struct {
            uint32_t res:10;
            uint32_t tiny:22;
        };
        struct {
            uint32_t _res:6;
            uint32_t ap1:2;
            uint32_t ap2:2;
            uint32_t ap3:2;
            uint32_t small:20;
        };
        struct {
            uint32_t __res:6;
            uint32_t _ap1:2;
            uint32_t _ap2:2;
            uint32_t _ap3:2;
            uint32_t __res1:4;
            uint32_t large:16;
        };
        uint32_t val;
    };
} __packed;

#define PDE_TYPE_FINE       0x03    // Indicates that this is a fine page table descriptor
#define PDE_TYPE_COARSE     0x01    // Indicates that this is a coarse page table descriptor
#define PDE_TYPE_SECTION    0x02    // Indicates that this is a section descriptor

#define PTE_TYPE_TINY       0x03
#define PTE_TYPE_SMALL      0x02
#define PTE_TYPE_LARGE      0x01

#define PAGE_B_THROUGH      0x00
#define PAGE_B_BACK         0x01

#define PAGE_C_CACHEOFF     0x00
#define PAGE_C_CACHEON      0x01

#define PAGE_AP_SYSTEM      0x01    // Privileged permissions
#define PAGE_AP_USER        0x03    // User permissions

#define PAGE_DOMAIN_NONE    0x00    // Any access generates a domain fault
#define PAGE_DOMAIN_CLIENT  0x01    // Accesses are checked against the access permission bits
#define PAGE_DOMAIN_MANAGER 0x03    // Accesses are not checked against the access permission bits

#endif /* __ASSEMBLY__ */
#endif /* _ARCH_ARM_PAGE_H_ */
