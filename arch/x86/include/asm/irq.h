/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IRQ_H
#define _ASM_X86_IRQ_H

#include <types.h>

#ifndef __ASSEMBLY__

typedef size_t irqflags_t;
typedef size_t irqnr_t;

static inline void arch_irq_disable()
{
    asm volatile(
        "cli"
        :::"memory"
    );
}

static inline void arch_irq_enable()
{
    asm volatile(
        "sti"
        :::"memory"
    );
}

static inline irqflags_t arch_irq_save(void)
{
    arch_irq_disable();
    return 0;
}

static inline void arch_irq_restore(irqflags_t flags)
{
    arch_irq_enable();
}


#endif /* __ASSEMBLY__ */
#endif  /* _ASM_X86_IRQ_H */
