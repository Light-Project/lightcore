#ifndef _ASM_CSKY_IRQ_H
#define _ASM_CSKY_IRQ_H

#include <types.h>
#include <asm/reg_ops.h>

#define irqflags_t  size_t

static inline void arch_irq_disable()
{
    asm volatile(
        "psrclr ie\n"
        :::"memory"
    );
}

static inline void arch_irq_enable()
{
    asm volatile(
        "psrset ee, ie\n"
        :::"memory"
    );
}

static inline irqflags_t arch_irq_save(void)
{
    irqflags_t flags;
    flags = mfcr("psr");
    arch_irq_disable();
    return flags;
}

static inline void arch_irq_restore(irqflags_t flags)
{
    mtcr("psr", flags);
}


#endif
