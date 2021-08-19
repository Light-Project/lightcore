/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_ARM_IRQ_H
#define _ASM_ARM_IRQ_H

#include <types.h>
#include <asm/regs.h>
#include <asm-generic/irq.h>

static inline void arch_irq_disable(void)
{
    asm volatile(
        "mrs    ip, cpsr\n\t"
        "orr    ip, #0xc0\n\t"
        "msr    cpsr, ip"
        :::"ip"
    );
}

static inline void arch_irq_enable(void)
{
    asm volatile(
        "mrs    ip, cpsr\n\t"
        "bic    ip, #0xc0\n\t"
        "msr    cpsr, ip"
        :::"ip"
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

#endif  /* _ASM_ARM_IRQ_H */
