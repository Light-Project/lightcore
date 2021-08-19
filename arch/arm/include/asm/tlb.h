/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_ARM_TLB_H_
#define _ASM_ARM_TLB_H_

static inline void tlb_inval_all(void)
{
    asm volatile (
        "eor    r0, r0\n"
        "mcr    p15, 0, r0, c8, c7, 0\n"
        ::: "r0"
    );
}

static inline void tlb_inval(uint32_t entry)
{
    asm volatile (
        "mcr    p15, 0, %0, c8, c7, 1\n"
        ::"r"(entry)
    );
}

static inline void tlb_inval_range(size_t addr, size_t size)
{

}

static inline void itlb_inval_all(void)
{
    asm volatile (
        "eor    r0, r0\n"
        "mcr    p15, 0, r0, c8, c5, 0\n"
        ::: "r0"
    );
}

static inline void itlb_inval(uint32_t entry)
{
    asm volatile (
        "mcr    p15, 0, %0, c8, c5, 1\n\t"
        ::"r"(entry)
    );
}

static inline void dtlb_inval_all(void)
{
    asm volatile (
        "eor    r0, r0\n"
        "mcr    p15, 0, r0, c8, c6, 0\n\t"
        ::: "r0"
    );
}

static inline void dtlb_inval(size_t entry)
{
    asm volatile (
        "mcr    p15, 0, %0, c8, c6, 1\n\t"
        ::"r"(entry)
    );
}

static inline uint32_t arch_tlb_rlock(void)
{
    uint32_t val;
    asm volatile (
        "mrc    p15, 0, %0, c10, c0, 1\n\t"
        :"=r"(val)::"memory"
    );
    return val;
}

static inline void arch_tlb_wlock(uint32_t lock)
{
    asm volatile (
        "mcr    p15, 0, %0, c10, c0, 1\n\t"
        ::"r"(lock)
    );
}

#endif /* _ASM_ARM_TLB_H_ */
