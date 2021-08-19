/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_PAGE_H_
#define _ARCH_X86_PAGE_H_

#include <types.h>

#ifndef __ASSEMBLY__

struct pde{
    union {
        struct {                /* 4KiB page */
            uint32_t p:1;       // Page present (0:non-existent 1:In memory)
            uint32_t rw:1;      // Read/write if 0, writes may not be allowed
            uint32_t us:1;      // User/supervisor: if 0, user-mode accesses are not allowed
            uint32_t pwt:1;     // Page-level write-through
            uint32_t pcd:1;     // Page-level cache disable
            uint32_t a:1;       // Accessed indicates whether software has accessed
            uint32_t d:1;       // Dirty indicates whether software has written (4MB page only)
            uint32_t ps:1;      // Page size (0:4KB 1:4MB)
            uint32_t g:1;       // Global; if CR4.PGE = 1, determines whether the translation is global
            uint32_t RES0:3;    //
            uint32_t addr:20;   // Physical address of 4-KByte aligned page table referenced by this entry
        };
        struct {                /* 4MiB page */
            uint32_t __p:1;     // Page present (0:non-existent 1:In memory)
            uint32_t __rw:1;    // Read/write if 0, writes may not be allowed
            uint32_t __us:1;    // User/supervisor: if 0, user-mode accesses are not allowed
            uint32_t __pwt:1;   // Page-level write-through
            uint32_t __pcd:1;   // Page-level cache disable
            uint32_t __a:1;     // Accessed indicates whether software has accessed
            uint32_t __d:1;     // Dirty indicates whether software has written (4MB page only)
            uint32_t __ps:1;    // Page size (0:4KB 1:4MB)
            uint32_t __g:1;     // Global; if CR4.PGE = 1, determines whether the translation is global
            uint32_t __RES0:3;  //
            uint32_t pat:1;     //
            uint32_t addrh:4;   // Physical address of 4-KByte aligned page table referenced by this entry
            uint32_t RES1:5;    //
            uint32_t addrl:10;  // Physical address of 4-KByte aligned page table referenced by this entry
        };
        uint32_t val;
    };
} __packed;

struct pte{
    union {
        struct {
            uint32_t p:1;       // Page present (0:non-existent 1:In memory)
            uint32_t rw:1;      // Read/write if 0, writes may not be allowed
            uint32_t us:1;      // User/supervisor: if 0, user-mode accesses are not allowed
            uint32_t pwt:1;     // Page-level write-through
            uint32_t pcd:1;     // Page-level cache disable
            uint32_t a:1;       // Accessed indicates whether software has accessed
            uint32_t d:1;       // Dirty indicates whether software has written (4MB page only)
            uint32_t pat:1;     // Page size (0:4KB 1:4MB)
            uint32_t g:1;       // Global; if CR4.PGE = 1, determines whether the translation is global
            uint32_t RES0:3;    // Ignored
            uint32_t addr:20;   // Physical address of the 4-KByte page referenced by this entry
        };
        uint32_t val;
    };
} __packed;

#define PAGE_RW_R           0x00
#define PAGE_RW_RW          0x01

#define PAGE_US_SUPER       0x00
#define PAGE_US_USER        0x01

#define PAGE_PWT_BACK       0x00
#define PAGE_PWT_THROUGH    0x01

#define PAGE_PCD_CACHEON    0x00
#define PAGE_PCD_CACHEOFF   0x01

#define PAGE_PS_4K          0x00
#define PAGE_PS_4M          0x01

#define PAGE_PDT_OFF        0x00
#define PAGE_PDT_ON         0x01

#define PAGE_PDT_OFF        0x00
#define PAGE_PDT_ON         0x01

#endif  /* __ASSEMBLY__ */
#endif  /* _ARCH_X86_PAGE_H_ */
