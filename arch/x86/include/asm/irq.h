/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IRQ_H
#define _ASM_X86_IRQ_H

#include <types.h>
#include <asm/idt.h>

#ifndef __ASSEMBLY__

enum isa_vector {
    I8253_IRQ       = IRQ_EXTERNAL + 0x00,  /* System Timer                 */
    ATKBD_IRQ       = IRQ_EXTERNAL + 0x01,  /* PS2 Keyboard                 */
    PIC2_IRQ        = IRQ_EXTERNAL + 0x02,  /* Cascade Controller           */
    COM24_IRQ       = IRQ_EXTERNAL + 0x03,  /* COM 2 and 4                  */
    COM13_IRQ       = IRQ_EXTERNAL + 0x04,  /* COM 1 and 3                  */
    SND_IRQ         = IRQ_EXTERNAL + 0x05,  /* Sound, Parallel port 2       */
    FLOPPY_IRQ      = IRQ_EXTERNAL + 0x06,  /* Floppy                       */
    PPC_IRQ         = IRQ_EXTERNAL + 0x07,  /* Parallel Port 1              */
    RTC_IRQ         = IRQ_EXTERNAL + 0x08,  /* Real-time clock              */
    NETWORK_IRQ     = IRQ_EXTERNAL + 0x09,  /* Network Available            */
/* Open                                             */
/* Open, SCSI, Video                                */
    PSMSE_IRQ       = IRQ_EXTERNAL + 0x0c,  /* PS2 Mouse                    */
/* Coprocessor                                      */
/* Open, Primary hard drive, Hard drive controller  */
/* Open, 2nd hard drive (secondary)                 */

};

static inline void cpu_irq_disable(void)
{
    asm volatile (
        "cli"
        :::"memory"
    );
}

static inline void cpu_irq_enable(void)
{
    asm volatile (
        "sti"
        :::"memory"
    );
}

static inline irqflags_t cpu_irq_save(void)
{
    cpu_irq_disable();
    return 0;
}

static inline void cpu_irq_restore(irqflags_t flags)
{
    cpu_irq_enable();
}

#endif /* __ASSEMBLY__ */
#endif  /* _ASM_X86_IRQ_H */
