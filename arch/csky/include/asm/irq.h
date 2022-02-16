/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_IRQ_H
#define _ASM_CSKY_IRQ_H

#include <types.h>
#include <asm/regs.h>
#include <arch/csky/interrupt.h>
#include <asm-generic/irq.h>

static inline void irq_local_disable()
{
    asm volatile(
        "psrclr ie\n"
        :::"memory"
    );
}

static inline void irq_local_enable()
{
    asm volatile(
        "psrset ee, ie\n"
        :::"memory"
    );
}

static inline irqflags_t irq_local_save(void)
{
    irqflags_t flags = mfcr("psr");
    irq_local_disable();
    return flags;
}

static inline void irq_local_restore(irqflags_t flags)
{
    mtcr("psr", flags);
}

void arch_irq_init(void);

#endif
