/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_ARM_IRQ_H
#define _ASM_ARM_IRQ_H

#include <types.h>
#include <asm/regs.h>
#include <asm-generic/irq.h>
#include <arch/arm/interrupt.h>

static inline void irq_local_disable(void)
{
    asm volatile(
        "mrs    ip, cpsr\n"
        "orr    ip, #0xc0\n"
        "msr    cpsr, ip"
        :::"ip"
    );
}

static inline void irq_local_enable(void)
{
    asm volatile(
        "mrs    ip, cpsr\n"
        "bic    ip, #0xc0\n"
        "msr    cpsr, ip"
        :::"ip"
    );
}

static inline irqflags_t irq_local_save(void)
{
    irq_local_disable();
    return 0;
}

static inline void irq_local_restore(irqflags_t flags)
{
    irq_local_enable();
}

void arch_irq_set(void(*)(struct regs *));

#endif  /* _ASM_ARM_IRQ_H */
