/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_E820_H_
#define _ASM_X86_E820_H_

#include <types.h>

#define E820_MAX_ENTRIES 128

#ifndef __ASSEMBLY__

enum e820_type {
    E820_TYPE_RAM               = 1,
    E820_TYPE_RESERVED          = 2,
    E820_TYPE_ACPI              = 3,
    E820_TYPE_NVS               = 4,
    E820_TYPE_UNUSABLE          = 5,
    E820_TYPE_PMEM              = 7,
    E820_TYPE_PRAM              = 12,
};

struct e820_entry {
    uint64_t addr;
    uint64_t size;
    uint32_t type;
} __packed;

struct e820_table {
    uint32_t entry_nr;
    struct e820_entry entry[E820_MAX_ENTRIES];
} __packed;

extern void e820_init(struct e820_table *);

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_X86_E820_H_ */
