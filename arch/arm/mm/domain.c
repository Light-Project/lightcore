#include <arch.h>


uint32_t arch_domain_read()
{
    uint32_t val;
    asm volatile
    (
        "mrc p15, 0, %0, c3, c0, 0\n\t"
        :"=r"(val)::"memory"
    );
    return val;
}

void arch_domain_write(uint32_t entry)
{
    asm volatile(
        "mcr p15, 0, %0, c3, c0, 0\n\t"
        ::"r"(entry)
    );
}
