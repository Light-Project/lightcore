/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IRQ_H
#define _ASM_X86_IRQ_H

#include <types.h>
#include <asm/idt.h>

#ifndef __ASSEMBLY__

enum isa_vector {
    I8253_IRQ       = 0x30, // System Timer
    I8042_IRQ       = 0x31, // Keyboard
    COM24_IRQ       = 0x33, // COM 2 and 4
    COM13_IRQ       = 0x34, // COM 1 and 3
    SND_IRQ         = 0x35, // Sound, Parallel port 2
    FLOPPY_IRQ      = 0x36, // Floppy
    PPC_IRQ         = 0x37, // Parallel Port 1

    RTC_IRQ         = 0x38, // Real-time clock  
    NETWORK_IRQ     = 0x39, // Redirected IRQ 2, Open, Network Available
};

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

void arch_irq_set(irqnr_t vector);

#endif /* __ASSEMBLY__ */
#endif  /* _ASM_X86_IRQ_H */
