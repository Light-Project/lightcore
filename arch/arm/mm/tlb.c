#include <arch.h>

void arch_tlb_inval()
{
    asm volatile(
        "mov r0, #0\n\t"
        "mcr p15, 0, r0, c8, c7, 0\n\t"
        ::: "r0"
    );
}

void arch_tlb_sinval(uint32_t entry)
{
    asm volatile(
        "mcr p15, 0, %0, c8, c7, 1\n\t"
        ::"r"(entry)
    );
}

void arch_itlb_inval()
{
    asm volatile(
        "mov r0, #0\n\t"
        "mcr p15, 0, r0, c8, c5, 0\n\t"
        ::: "r0"
    );
}

void arch_itlb_sinval(uint32_t entry)
{
    asm volatile(
        "mcr p15, 0, %0, c8, c5, 1\n\t"
        ::"r"(entry)
    );
}

void arch_dtlb_inval()
{
    asm volatile(
        "mov r0, #0\n\t"
        "mcr p15, 0, r0, c8, c6, 0\n\t"
        ::: "r0"
    );
}

void arch_dtlb_sinval(uint32_t entry)
{
    asm volatile(
        "mcr p15, 0, %0, c8, c6, 1\n\t"
        ::"r"(entry)
    );
}

uint32_t arch_tlb_rlock()
{
    uint32_t val;
    asm volatile
    (
        "mrc p15, 0, %0, c10, c0, 1\n\t"
        :"=r"(val)::"memory"
    );
    return val;
}

void arch_tlb_wlock(uint32_t lock)
{
    asm volatile
    (
        "mcr p15, 0, %0, c10, c0, 1\n\t"
        ::"r"(lock)
    );
}
