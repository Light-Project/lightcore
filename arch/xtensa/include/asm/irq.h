#ifndef _ASM_XTENSA_IRQ_H
#define _ASM_XTENSA_IRQ_H

#include <types.h>

#define irqflags_t  size_t

static inline void cpu_irq_disable()
{
    unsigned long flags;
    asm volatile(
        "rsil  %0, 15"
        :"=a" (flags)::"memory"
    );
}

static inline void cpu_irq_enable()
{
    unsigned long flags;
    asm volatile(
        "rsil  %0, 0"
        :"=a" (flags)::"memory"
    );
}

static inline irqflags_t cpu_irq_save(void)
{
    irqflags_t flags;
    asm volatile(
        "rsil  %0, 15"
        :"=a" (flags)::"memory"
    );
    return flags;
}

static inline void cpu_irq_restore(irqflags_t flags)
{
    flags &= 0x0f;

    asm volatile(
        "wsr    %0, ps\n"
        "rsync \n"
        ::"a"(flags):"memory"
    );
}


#endif