/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_PAGE_H_
#define _ARCH_X86_PAGE_H_

#include <types.h>

#ifndef __ASSEMBLY__

#define PAGE_PRESENT        BIT(0)  /* is present */
#define PAGE_RW             BIT(1)  /* writeable */
#define PAGE_USER           BIT(2)  /* userspace addressable */
#define PAGE_PWT            BIT(3)  /* page write through */
#define PAGE_PCD            BIT(4)  /* page cache disabled */
#define PAGE_ACCESSED       BIT(5)  /* was accessed (raised by CPU) */
#define PAGE_DIRTY          BIT(6)  /* was written to (raised by CPU) */
#define PAGE_PSE            BIT(7)  /* 4 MB (or 2MB) page */
#define PAGE_PAT            BIT(7)  /* on 4KB pages */
#define PAGE_GLOBAL         BIT(8)  /* Global TLB entry PPro+ */
#define PAGE_SOFTW1         BIT(9)  /* available for programmer */
#define PAGE_SOFTW2         BIT(10) /* " */
#define PAGE_SOFTW3         BIT(11) /* " */
#define PAGE_PAT_LARGE      BIT(12) /* On 2MB or 1GB pages */
#define PAGE_SOFTW4         BIT(58) /* available for programmer */
#define PAGE_PKEY_BIT0      BIT(59) /* Protection Keys, bit 1/4 */
#define PAGE_PKEY_BIT1      BIT(60) /* Protection Keys, bit 2/4 */
#define PAGE_PKEY_BIT2      BIT(61) /* Protection Keys, bit 3/4 */
#define PAGE_PKEY_BIT3      BIT(62) /* Protection Keys, bit 4/4 */
#define PAGE_NX             BIT(63) /* No execute: only valid after cpuid check */

#if defined(CONFIG_ARCH_X86)

struct pgd {
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
            uint32_t res0:3;    //
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
            uint32_t __res0:3;  //
            uint32_t pat:1;     //
            uint32_t addrh:4;   // Physical address of 4-KByte aligned page table referenced by this entry
            uint32_t res1:5;    //
            uint32_t addrl:10;  // Physical address of 4-KByte aligned page table referenced by this entry
        };
    };
} __packed;

struct pte {
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
} __packed;

#elif defined(CONFIG_ARCH_X86_64)

struct pgd {
    uint64_t p:1;       // Page present (0:non-existent 1:In memory)
    uint64_t rw:1;      // Read/write if 0, writes may not be allowed
    uint64_t us:1;      // User/supervisor: if 0, user-mode accesses are not allowed
    uint64_t pwt:1;     // Page-level write-through
    uint64_t pcd:1;     // Page-level cache disable
    uint64_t a:1;       // Accessed indicates whether software has accessed
    uint64_t res0:1;    // Ignored
    uint64_t pat:1;     // Page size (0:4KB 1:4MB)
    uint64_t g:1;       // Global; if CR4.PGE = 1, determines whether the translation is global
    uint64_t res1:3;    // Ignored
    uint64_t addr:40;   // Physical address of the 4-KByte page referenced by this entry
    uint64_t res2:11;   // Ignored
    uint64_t xd3:1;     //
};

struct p4d {
    uint64_t p:1;       // Page present (0:non-existent 1:In memory)
    uint64_t rw:1;      // Read/write if 0, writes may not be allowed
    uint64_t us:1;      // User/supervisor: if 0, user-mode accesses are not allowed
    uint64_t pwt:1;     // Page-level write-through
    uint64_t pcd:1;     // Page-level cache disable
    uint64_t a:1;       // Accessed indicates whether software has accessed
    uint64_t res0:1;    // Ignored
    uint64_t pat:1;     // Page size (0:4KB 1:4MB)
    uint64_t g:1;       // Global; if CR4.PGE = 1, determines whether the translation is global
    uint64_t res1:3;    // Ignored
    uint64_t addr:40;   // Physical address of the 4-KByte page referenced by this entry
    uint64_t res2:11;   // Ignored
    uint64_t xd3:1;     //
};

struct pud {
    union {
        struct {
            uint64_t p:1;       // Page present (0:non-existent 1:In memory)
            uint64_t rw:1;      // Read/write if 0, writes may not be allowed
            uint64_t us:1;      // User/supervisor: if 0, user-mode accesses are not allowed
            uint64_t pwt:1;     // Page-level write-through
            uint64_t pcd:1;     // Page-level cache disable
            uint64_t a:1;       // Accessed indicates whether software has accessed
            uint64_t res0:1;    // Ignored
            uint64_t pat:1;     // Page size (0:4KB 1:4MB)
            uint64_t g:1;       // Global; if CR4.PGE = 1, determines whether the translation is global
            uint64_t res1:3;    // Ignored
            uint64_t addr:40;   // Physical address of the 4-KByte page referenced by this entry
            uint64_t res2:11;   // Ignored
            uint64_t xd3:1;     //
        };
    };
};

struct pmd {
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
        struct {                /* 2MiB page */
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
    };
};

struct pte {
    uint64_t p:1;       // Page present (0:non-existent 1:In memory)
    uint64_t rw:1;      // Read/write if 0, writes may not be allowed
    uint64_t us:1;      // User/supervisor: if 0, user-mode accesses are not allowed
    uint64_t pwt:1;     // Page-level write-through
    uint64_t pcd:1;     // Page-level cache disable
    uint64_t a:1;       // Accessed indicates whether software has accessed
    uint64_t d:1;       // Dirty indicates whether software has written (4MB page only)
    uint64_t pat:1;     // Page size (0:4KB 1:4MB)
    uint64_t g:1;       // Global; if CR4.PGE = 1, determines whether the translation is global
    uint64_t RES0:3;    // Ignored
    uint64_t addr:40;   // Physical address of the 4-KByte page referenced by this entry
} __packed;

#endif  /* CONFIG_ARCH_X86_64 */

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
