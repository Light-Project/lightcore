/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_PAGE_H_
#define _ARCH_X86_PAGE_H_

#include <types.h>
#include <bits.h>

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

#ifndef __ASSEMBLY__


#if defined(CONFIG_ARCH_X86_32)

typedef unsigned long pgdval_t;
typedef unsigned long pmdval_t;
typedef unsigned long pteval_t;

struct pgd {
    union {
        struct {                /* 4KiB page */
            pgdval_t p:1;       /* 00: Page present (0:non-existent 1:In memory) */
            pgdval_t rw:1;      /* 01: Read/write if 0, writes may not be allowed */
            pgdval_t us:1;      /* 02: User/supervisor: if 0, user-mode accesses are not allowed */
            pgdval_t pwt:1;     /* 03: Page-level write-through */
            pgdval_t pcd:1;     /* 04: Page-level cache disable */
            pgdval_t a:1;       /* 05: Accessed indicates whether software has accessed */
            pgdval_t d:1;       /* 06: Dirty indicates whether software has written (huge page only) */
            pgdval_t ps:1;      /* 07: Page size */
            pgdval_t g:1;       /* 08: Global; if CR4.PGE = 1, determines whether the translation is global */
            pgdval_t res0:3;    /* 09: Ignored */
            pgdval_t addr:20;   /* 12: Physical address of 4-KByte aligned page table referenced by this entry */
        };
        struct {                /* Huge page */
            pgdval_t __p:1;     /* 00: Page present (0:non-existent 1:In memory) */
            pgdval_t __rw:1;    /* 01: Read/write if 0, writes may not be allowed */
            pgdval_t __us:1;    /* 02: User/supervisor: if 0, user-mode accesses are not allowed */
            pgdval_t __pwt:1;   /* 03: Page-level write-through */
            pgdval_t __pcd:1;   /* 04: Page-level cache disable */
            pgdval_t __a:1;     /* 05: Accessed indicates whether software has accessed */
            pgdval_t __d:1;     /* 06: Dirty indicates whether software has written (huge page only) */
            pgdval_t __ps:1;    /* 07: Page size */
            pgdval_t __g:1;     /* 08: Global; if CR4.PGE = 1, determines whether the translation is global */
            pgdval_t __res0:3;  /* 09: Ignored */
            pgdval_t pat:1;     /* 12: Indirectly determines the memory type used to access the page referenced by this entry */
            pgdval_t addrh:4;   /* 13: Physical address of 4-KByte aligned page table referenced by this entry */
            pgdval_t res1:5;    /* 17: Indirectly determines the memory type used to access the 4-KByte page referenced by this entry */
            pgdval_t addrl:10;  /* 22: Physical address of 4-KByte aligned page table referenced by this entry */
        };
        pgdval_t val;
    };
} __packed;

struct pte {
    union {
        struct {
            pteval_t p:1;       /* 00: Page present (0:non-existent 1:In memory) */
            pteval_t rw:1;      /* 01: Read/write if 0, writes may not be allowed */
            pteval_t us:1;      /* 02: User/supervisor: if 0, user-mode accesses are not allowed */
            pteval_t pwt:1;     /* 03: Page-level write-through */
            pteval_t pcd:1;     /* 04: Page-level cache disable */
            pteval_t a:1;       /* 05: Accessed indicates whether software has accessed */
            pteval_t d:1;       /* 06: Dirty indicates whether software has written (4MB page only) */
            pteval_t pat:1;     /* 07: Indirectly determines the memory type used to access the page referenced by this entry */
            pteval_t g:1;       /* 08: Global; if CR4.PGE = 1, determines whether the translation is global */
            pteval_t RES0:3;    /* 09: Ignored */
            pteval_t addr:20;   /* 12: Physical address of the 4-KByte page referenced by this entry */
        };
        pteval_t val;
    };
} __packed;

#elif defined(CONFIG_ARCH_X86_64)

typedef unsigned long long pgdval_t;
typedef unsigned long long p4dval_t;
typedef unsigned long long pudval_t;
typedef unsigned long long pmdval_t;
typedef unsigned long long pteval_t;

struct pgd {
    union {
        struct {                /* Normal Page */
            pgdval_t p:1;       /* 00: Page present (0:non-existent 1:In memory) */
            pgdval_t rw:1;      /* 01: Read/write if 0, writes may not be allowed */
            pgdval_t us:1;      /* 02: User/supervisor: if 0, user-mode accesses are not allowed */
            pgdval_t pwt:1;     /* 03: Page-level write-through */
            pgdval_t pcd:1;     /* 04: Page-level cache disable */
            pgdval_t a:1;       /* 05: Accessed indicates whether software has accessed */
            pgdval_t d:1;       /* 06: Dirty indicates whether software has written (huge page only) */
            pgdval_t ps:1;      /* 07: Page size */
            pgdval_t g:1;       /* 08: Global; if CR4.PGE = 1, determines whether the translation is global */
            pgdval_t res0:3;    /* 09: Ignored */
            pgdval_t addr:40;   /* 12: Physical address of 4-KByte aligned page table referenced by this entry */
        };
        struct {                /* Huge Page */
            pgdval_t __p:1;     /* 00: Page present (0:non-existent 1:In memory) */
            pgdval_t __rw:1;    /* 01: Read/write if 0, writes may not be allowed */
            pgdval_t __us:1;    /* 02: User/supervisor: if 0, user-mode accesses are not allowed */
            pgdval_t __pwt:1;   /* 03: Page-level write-through */
            pgdval_t __pcd:1;   /* 04: Page-level cache disable */
            pgdval_t __a:1;     /* 05: Accessed indicates whether software has accessed */
            pgdval_t __d:1;     /* 06: Dirty indicates whether software has written (huge page only) */
            pgdval_t __ps:1;    /* 07: Page size */
            pgdval_t __g:1;     /* 08: Global; if CR4.PGE = 1, determines whether the translation is global */
            pgdval_t __res0:3;  /* 09: Ignored */
            pgdval_t pat:1;     /* 12: Indirectly determines the memory type used to access the page referenced by this entry */
            pgdval_t __res1:8;  /* 13: Ignored */
            pgdval_t addrl:31;  /* 21: Physical address of 4-KByte aligned page table referenced by this entry */
            pgdval_t __res2:7;  /* 52: Ignored */
            pgdval_t prot:4;    /* 59: Protection key; if CR4.PKE = 1 or CR4.PKS = 1 */
            pgdval_t xd:1;      /* 63: If IA32_EFER.NXE = 1, execute-disable */
        };
        pgdval_t val;
    };
} __packed;

struct pte {
    union {
        struct {
            pteval_t p:1;       /* 00: Page present (0:non-existent 1:In memory) */
            pteval_t rw:1;      /* 01: Read/write if 0, writes may not be allowed */
            pteval_t us:1;      /* 02: User/supervisor: if 0, user-mode accesses are not allowed */
            pteval_t pwt:1;     /* 03: Page-level write-through */
            pteval_t pcd:1;     /* 04: Page-level cache disable */
            pteval_t a:1;       /* 05: Accessed indicates whether software has accessed */
            pteval_t d:1;       /* 06: Dirty indicates whether software has written (4MB page only) */
            pteval_t pat:1;     /* 07: Indirectly determines the memory type used to access the page referenced by this entry */
            pteval_t g:1;       /* 08: Global; if CR4.PGE = 1, determines whether the translation is global */
            pteval_t res0:3;    /* 09: Ignored */
            pteval_t addr:40;   /* 12: Physical address of the 4-KByte page referenced by this entry */
            pteval_t res1:7;    /* 52: Ignored */
            pteval_t prot:4;    /* 59: Protection key; if CR4.PKE = 1 or CR4.PKS = 1 */
            pteval_t xd:1;      /* 63: If IA32_EFER.NXE = 1, execute-disable */
        };
        pteval_t val;
    };
} __packed;

#endif /* CONFIG_ARCH_X86_64 */
#endif /* __ASSEMBLY__ */

#define PAGE_RW_R           0x00
#define PAGE_RW_RW          0x01
#define PAGE_US_SUPER       0x00
#define PAGE_US_USER        0x01
#define PAGE_PWT_BACK       0x00
#define PAGE_PWT_THROUGH    0x01
#define PAGE_PCD_CACHEON    0x00
#define PAGE_PCD_CACHEOFF   0x01
#define PAGE_PS_NORMAL      0x00
#define PAGE_PS_HUGE        0x01
#define PAGE_PDT_OFF        0x00
#define PAGE_PDT_ON         0x01

#endif /* _ARCH_X86_PAGE_H_ */
