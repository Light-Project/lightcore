/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IRQ_H
#define _ASM_X86_IRQ_H

#include <types.h>
#include <asm/idt.h>

#ifndef __ASSEMBLY__

enum isa_vector {
    I8253_IRQ       = IRQ_EXTERNAL + 0x00,  // System Timer
    PIC2_IRQ        = IRQ_EXTERNAL + 0x01,  // Keyboard
    I8042_IRQ       = IRQ_EXTERNAL + 0x02,  // PIC 2
    COM24_IRQ       = IRQ_EXTERNAL + 0x03,  // COM 2 and 4
    COM13_IRQ       = IRQ_EXTERNAL + 0x04,  // COM 1 and 3
    SND_IRQ         = IRQ_EXTERNAL + 0x05,  // Sound, Parallel port 2
    FLOPPY_IRQ      = IRQ_EXTERNAL + 0x06,  // Floppy
    PPC_IRQ         = IRQ_EXTERNAL + 0x07,  // Parallel Port 1
    RTC_IRQ         = IRQ_EXTERNAL + 0x08,  // Real-time clock  
    NETWORK_IRQ     = IRQ_EXTERNAL + 0x09,  // Redirected IRQ 2, Open, Network Available
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
