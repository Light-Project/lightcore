#include <arch.h>

void icache_enable()
{
    asm volatile
    (
        "mrc    p15, 0, r0, c1, c0, 0\n\t"
        "orr    r0, r0, #(1<<12)\n\t"
        "mcr    p15, 0, r0, c1, c0, 0\n\t"
        :::"r0"
    );
}

void icache_disable()
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0\n\t"
        "bic r0, r0, #(1<<12)\n\t"
        "mcr p15, 0, r0, c1, c0, 0\n\t"
        :::"r0"
    );
}


void dcache_enable()
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0\n\t"
        "orr r0, r0, #(1<<2)\n\t"
        "mcr p15, 0, r0, c1, c0, 0\n\t"
        :::"r0"
    );
}

void dcache_disable()
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0\n\t"
        "bic r0, r0, #(1<<2)\n\t"
        "mcr p15, 0, r0, c1, c0, 0\n\t"
        :::"r0"
    );
}

void cache_inval()
{
    asm volatile
    (
        "mov    r0, #0\n\t"
        "mcr p15, 0, r0, c7, c7, 0\n\t"
        :::"r0"
    );
}

void icache_inval()
{
    asm volatile
    (
        "mov    r0, #0\n\t"
        "mcr    p15, 0, r0, c7, c5, 0\n\t"
        :::"r0"
    );
}

void icache_sinval(uint32_t entry)
{
    asm volatile
    (
        "mcr p15, 0, %0, c7, c5, 1\n\t"
        ::"r"(entry)
    );
}

void icache_winval(uint32_t entry)
{
    asm volatile
    (
        "mcr p15, 0, %0, c7, c5, 2\n\t"
        ::"r"(entry)
    );
}

void icache_perfetch(uint32_t entry)
{
    asm volatile
    (
        "mcr p15, 0, %0, c7, c13, 1\n\t"
        ::"r"(entry)
    );
}

void dcache_inval()
{
    asm volatile
    (
        "mov    r0, #0\n\t"
        "mcr    p15, 0, r0, c7, c6, 0\n\t"
        :::"r0"
    );
}

void dcache_sinval(uint32_t entry)
{
    asm volatile
    (
        "mcr p15, 0, %0, c7, c6, 1\n\t"
        ::"r"(entry)
    );
}

void dcache_winval(uint32_t entry)
{
    asm volatile
    (
        "mcr p15, 0, %0, c7, c6, 2\n\t"
        ::"r"(entry)
    );
}

void dcache_sclean(uint32_t entry)
{
    asm volatile
    (
        "mcr p15, 0, %0, c7, c10, 1\n\t"
        ::"r"(entry)
    );
}

void dcache_wclean(uint32_t entry)
{
    asm volatile
    (
        "mcr p15, 0, %0, c7, c10, 2\n\t"
        ::"r"(entry)
    );
}

uint32_t dcache_testclean()
{
    uint32_t val;
    asm volatile
    (
        "mrc    p15, 0, %0, c7, c10, 3\n\t"
        :"=r"(val)::"memory"
    );
    return val;
}

void dcache_cleaninval(uint32_t entry)
{
    asm volatile
    (
        "mcr p15, 0, %0, c7, c14, 1\n\t"
        ::"r"(entry)
    );
}

void dcache_scleaninval(uint32_t entry)
{
    asm volatile
    (
        "mcr p15, 0, %0, c7, c14, 2\n\t"
        ::"r"(entry)
    );
}

uint32_t dcache_testcleaninval()
{
    uint32_t val;
    asm volatile
    (
        "mrc    p15, 0, %0, c7, c14, 3\n\t"
        :"=r"(val)::"memory"
    );
    return val;
}

void dcache_drain()
{
    asm volatile
    (
        "mov    r0, #0\n\t"
        "mcr    p15, 0, r0, c7, c10, 4\n\t"
        :::"r0"
    );
}

void dcache_wait_interrupt()
{
    asm volatile
    (
        "mov    r0, #0\n\t"
        "mcr    p15, 0, r0, c7, c0, 4\n\t"
        :::"r0"
    );
}

uint32_t dcache_rlock()
{
    uint32_t val;
    asm volatile
    (
        "mrc p15, 0, %0, c9, c0, 0\n\t"
        :"=r"(val)::"memory"
    );
    return val;
}

void dcache_wlock(uint32_t lock)
{
    asm volatile
    (
        "mcr p15, 0, %0, c9, c0, 0\n\t"
        ::"r"(lock)
    );
}

uint32_t icache_rlock()
{
    uint32_t val;
    asm volatile
    (
        "mrc p15, 0, %0, c9, c0, 1\n\t"
        :"=r"(val)::"memory"
    );
    return val;
}

void icache_wlock(uint32_t lock)
{
    asm volatile
    (
        "mcr p15, 0, %0, c9, c0, 1\n\t"
        ::"r"(lock)
    );
}
