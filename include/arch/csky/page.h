/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ARCH_CSKY_PAGE_H_
#define _ARCH_CSKY_PAGE_H_

#include <types.h>

#ifndef __ASSEMBLY__

struct pgd {
    phys_addr_t pte;
} __packed;

struct pte {
    union {
        struct {
            uint32_t res:6;
            uint32_t g:1;
            uint32_t v:1;
            uint32_t d:1;
            uint32_t cache:3;
            uint32_t addr:20;
        };
        uint32_t val;
    };
} __packed;

#define PAGE_CACHE      0x03
#define PAGE_UNCACHE    0x02

#endif /* __ASSEMBLY__ */
#endif /* _ARCH_ARM_PAGE_H_ */
