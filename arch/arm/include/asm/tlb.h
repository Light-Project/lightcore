/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_ARM_TLB_H_
#define _ASM_ARM_TLB_H_

#include <types.h>
#include <kernel.h>
#include <asm/page.h>

#ifndef CONFIG_SMP

static inline void tlb_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c8, c7, 0\n"
        ::: "ip"
    );
}

static inline void tlb_inval(uint32_t mva)
{
    asm volatile (
        "mcr    p15, 0, %0, c8, c7, 1\n"
        ::"r"(mva)
    );
}

static inline void itlb_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c8, c5, 0\n"
        ::: "ip"
    );
}

static inline void itlb_inval(uint32_t mva)
{
    asm volatile (
        "mcr    p15, 0, %0, c8, c5, 1\n\t"
        ::"r"(mva)
    );
}

static inline void dtlb_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c8, c6, 0\n\t"
        ::: "ip"
    );
}

static inline void dtlb_inval(uint32_t mva)
{
    asm volatile (
        "mcr    p15, 0, %0, c8, c6, 1\n\t"
        ::"r"(mva)
    );
}

#else /* !CONFIG_SMP */

static inline void tlb_inval_all(void)
{
    asm volatile (
        "eor    ip, ip\n"
        "mcr    p15, 0, ip, c8, c3, 0\n"
        ::: "ip"
    );
}

static inline void tlb_inval(uint32_t entry)
{
    asm volatile (
        "mcr    p15, 0, %0, c8, c3, 1\n"
        ::"r"(entry)
    );
}

#endif /* CONFIG_SMP */

void tlb_inval(uint32_t entry);
void itlb_inval_all(void);
void itlb_inval(uint32_t entry);
void dtlb_inval_all(void);
void dtlb_inval(uint32_t entry);

uint32_t tlb_rlock(void);
void tlb_wlock(uint32_t lock);

static inline void tlb_inval_range(size_t start, size_t end)
{
    uint32_t mva;

    start = align_low(start, PAGE_SIZE);
    end = align_high(end, PAGE_SIZE);

    for (mva = start; mva < end; mva += PAGE_SIZE)
        tlb_inval(mva);
}

#endif /* _ASM_ARM_TLB_H_ */
