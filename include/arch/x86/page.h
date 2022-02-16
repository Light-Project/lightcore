/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_PAGE_H_
#define _ARCH_X86_PAGE_H_

#include <types.h>

#ifndef __ASSEMBLY__

#define PAGE_PRESENT            BIT_ULL(0)  /* Is Present */
#define PAGE_RW                 BIT_ULL(1)  /* Writeable */
#define PAGE_USER               BIT_ULL(2)  /* Userspace Addressable */
#define PAGE_PWT                BIT_ULL(3)  /* Page Write Through */
#define PAGE_PCD                BIT_ULL(4)  /* Page Cache Disabled */
#define PAGE_ACCESSED           BIT_ULL(5)  /* Was Accessed (Raised By Cpu) */
#define PAGE_DIRTY              BIT_ULL(6)  /* Was Written To (Raised By Cpu) */
#define PAGE_PSE                BIT_ULL(7)  /* 4 Mb (Or 2mb) Page */
#define PAGE_PAT                BIT_ULL(7)  /* On 4kb Pages */
#define PAGE_GLOBAL             BIT_ULL(8)  /* Global Tlb Entry Ppro+ */
#define PAGE_SOFTW1             BIT_ULL(9)  /* Available for Programmer */
#define PAGE_SOFTW2             BIT_ULL(10) /* Available for Programmer */
#define PAGE_SOFTW3             BIT_ULL(11) /* Available for Programmer */
#define PAGE_PAT_LARGE          BIT_ULL(12) /* On 2MB or 1GB Pages */
#define PAGE_SOFTW4             BIT_ULL(58) /* Available for Programmer */
#define PAGE_PKEY_BIT0          BIT_ULL(59) /* Protection Keys, Bit 1/4 */
#define PAGE_PKEY_BIT1          BIT_ULL(60) /* Protection Keys, Bit 2/4 */
#define PAGE_PKEY_BIT2          BIT_ULL(61) /* Protection Keys, Bit 3/4 */
#define PAGE_PKEY_BIT3          BIT_ULL(62) /* Protection Keys, Bit 4/4 */
#define PAGE_NX                 BIT_ULL(63) /* No Execute: Only Valid After Cpuid Check */

#if defined(CONFIG_ARCH_X86_32)

struct pgd {
    union {
        struct {                /* 4KiB page */
            uint32_t p:1;       /* Page present (0:non-existent 1:In memory) */
            uint32_t rw:1;      /* Read/write if 0, writes may not be allowed */
            uint32_t us:1;      /* User/supervisor: if 0, user-mode accesses are not allowed */
            uint32_t pwt:1;     /* Page-level write-through */
            uint32_t pcd:1;     /* Page-level cache disable */
            uint32_t a:1;       /* Accessed indicates whether software has accessed */
            uint32_t d:1;       /* Dirty indicates whether software has written (4MB page only) */
            uint32_t ps:1;      /* Page size (0:4KB 1:4MB) */
            uint32_t g:1;       /* Global; if CR4.PGE = 1, determines whether the translation is global */
            uint32_t res0:3;    /* Ignored */
            uint32_t addr:20;   /* Physical address of 4-KByte aligned page table referenced by this entry */
        };
        struct {                /* Huge page */
            uint32_t __p:1;     /* Page present (0:non-existent 1:In memory) */
            uint32_t __rw:1;    /* Read/write if 0, writes may not be allowed */
            uint32_t __us:1;    /* User/supervisor: if 0, user-mode accesses are not allowed */
            uint32_t __pwt:1;   /* Page-level write-through */
            uint32_t __pcd:1;   /* Page-level cache disable */
            uint32_t __a:1;     /* Accessed indicates whether software has accessed */
            uint32_t __d:1;     /* Dirty indicates whether software has written (4MB page only) */
            uint32_t __ps:1;    /* Page size (0:4KB 1:4MB) */
            uint32_t __g:1;     /* Global; if CR4.PGE = 1, determines whether the translation is global */
            uint32_t __res0:3;  /* Ignored */
            uint32_t pat:1;     /* Ignored */
            uint32_t addrh:4;   /* Physical address of 4-KByte aligned page table referenced by this entry */
            uint32_t res1:5;    /* Ignored */
            uint32_t addrl:10;  /* Physical address of 4-KByte aligned page table referenced by this entry */
        };
        uint32_t val;
    };
} __packed;

struct pte {
    union {
        struct {
            uint32_t p:1;       /* Page present (0:non-existent 1:In memory) */
            uint32_t rw:1;      /* Read/write if 0, writes may not be allowed */
            uint32_t us:1;      /* User/supervisor: if 0, user-mode accesses are not allowed */
            uint32_t pwt:1;     /* Page-level write-through */
            uint32_t pcd:1;     /* Page-level cache disable */
            uint32_t a:1;       /* Accessed indicates whether software has accessed */
            uint32_t d:1;       /* Dirty indicates whether software has written (4MB page only) */
            uint32_t pat:1;     /* Page size (0:4KB 1:4MB) */
            uint32_t g:1;       /* Global; if CR4.PGE = 1, determines whether the translation is global */
            uint32_t RES0:3;    /* Ignored */
            uint32_t addr:20;   /* Physical address of the 4-KByte page referenced by this entry */
        };
        uint32_t val;
    };
} __packed;

#elif defined(CONFIG_ARCH_X86_64)

struct pgd {
    union {
        struct {                /* Normal Page */
            uint64_t p:1;       /* Page present (0:non-existent 1:In memory) */
            uint64_t rw:1;      /* Read/write if 0, writes may not be allowed */
            uint64_t us:1;      /* User/supervisor: if 0, user-mode accesses are not allowed */
            uint64_t pwt:1;     /* Page-level write-through */
            uint64_t pcd:1;     /* Page-level cache disable */
            uint64_t a:1;       /* Accessed indicates whether software has accessed */
            uint64_t d:1;       /* Dirty indicates whether software has written (4MB page only) */
            uint64_t ps:1;      /* Page size (0:4KB 1:4MB) */
            uint64_t g:1;       /* Global; if CR4.PGE = 1, determines whether the translation is global */
            uint64_t res0:3;    /* Ignored */
            uint64_t addr:20;   /* Physical address of 4-KByte aligned page table referenced by this entry */
        };
        struct {                /* Huge Page */
            uint64_t __p:1;     /* Page present (0:non-existent 1:In memory) */
            uint64_t __rw:1;    /* Read/write if 0, writes may not be allowed */
            uint64_t __us:1;    /* User/supervisor: if 0, user-mode accesses are not allowed */
            uint64_t __pwt:1;   /* Page-level write-through */
            uint64_t __pcd:1;   /* Page-level cache disable */
            uint64_t __a:1;     /* Accessed indicates whether software has accessed */
            uint64_t __d:1;     /* Dirty indicates whether software has written (4MB page only) */
            uint64_t __ps:1;    /* Page size (0:4KB 1:4MB) */
            uint64_t __g:1;     /* Global; if CR4.PGE = 1, determines whether the translation is global */
            uint64_t __res0:3;  /* Ignored */
            uint64_t pat:1;     /* Ignored */
            uint64_t addrh:4;   /* Physical address of 4-KByte aligned page table referenced by this entry */
            uint64_t res1:5;    /* Ignored */
            uint64_t addrl:10;  /* Physical address of 4-KByte aligned page table referenced by this entry */
        };
        uint64_t val;
    };
};

struct pte {
    union {
        struct {
            uint64_t p:1;       /* Page present (0:non-existent 1:In memory) */
            uint64_t rw:1;      /* Read/write if 0, writes may not be allowed */
            uint64_t us:1;      /* User/supervisor: if 0, user-mode accesses are not allowed */
            uint64_t pwt:1;     /* Page-level write-through */
            uint64_t pcd:1;     /* Page-level cache disable */
            uint64_t a:1;       /* Accessed indicates whether software has accessed */
            uint64_t d:1;       /* Dirty indicates whether software has written (4MB page only) */
            uint64_t pat:1;     /* Page size (0:4KB 1:4MB) */
            uint64_t g:1;       /* Global; if CR4.PGE = 1, determines whether the translation is global */
            uint64_t res0:3;    /* Ignored */
            uint64_t addr:40;   /* Physical address of the 4-KByte page referenced by this entry */
        };
        uint64_t val;
    };
} __packed;

#endif

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

#endif  /* __ASSEMBLY__ */
#endif  /* _ARCH_X86_PAGE_H_ */
