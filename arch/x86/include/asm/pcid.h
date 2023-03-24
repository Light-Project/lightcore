/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PCID_H_
#define _ASM_X86_PCID_H_

#include <types.h>

enum pcid_type {
    PCID_INDIV_ADDR         = 0x00,
    PCID_SINGLE_CTXT        = 0x01,
    PCID_ALL_INCL_GLOBAL    = 0x02,
    PCID_ALL_NON_GLOBAL     = 0x03,
};

struct pcid_desc {
    uint64_t pcid;
    uint64_t addr;
} __packed;

static inline void pcid_inval(enum pcid_type type, size_t pcid, size_t addr)
{
    struct pcid_desc desc = {pcid, addr};

    asm volatile (
        "invpcid %[desc], %[type]"
        :: [desc] "m" (desc), [type] "r" (type)
        : "memory"
    );
}

static inline void pcid_inval_one(size_t pcid, size_t addr)
{
    pcid_inval(PCID_INDIV_ADDR, pcid, addr);
}

static inline void pcid_inval_single_context(size_t pcid, size_t addr)
{
    pcid_inval(PCID_SINGLE_CTXT, pcid, addr);
}

static inline void pcid_inval_all(size_t pcid, size_t addr)
{
    pcid_inval(PCID_ALL_INCL_GLOBAL, pcid, addr);
}

static inline void pcid_inval_all_nonglobals(size_t pcid, size_t addr)
{
    pcid_inval(PCID_ALL_NON_GLOBAL, pcid, addr);
}

#endif /* _ASM_X86_PCID_H_ */
