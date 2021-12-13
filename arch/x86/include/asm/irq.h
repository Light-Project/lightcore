/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IRQ_H_
#define _ASM_X86_IRQ_H_

#include <types.h>

#define IRQ_EXTERNAL    0x20
#define IRQ_NR_MAX      256

#ifndef __ASSEMBLY__

enum isa_vector {
    I8253_IRQ       = IRQ_EXTERNAL + 0x00,  /* System Timer             */
    ATKBD_IRQ       = IRQ_EXTERNAL + 0x01,  /* PS2 Keyboard             */
    PIC2_IRQ        = IRQ_EXTERNAL + 0x02,  /* Cascade Controller       */
    COM24_IRQ       = IRQ_EXTERNAL + 0x03,  /* COM 2 and 4              */
    COM13_IRQ       = IRQ_EXTERNAL + 0x04,  /* COM 1 and 3              */
    PPC2_IRQ        = IRQ_EXTERNAL + 0x05,  /* Parallel port 2          */
    FLOPPY_IRQ      = IRQ_EXTERNAL + 0x06,  /* Floppy                   */
    PPC1_IRQ        = IRQ_EXTERNAL + 0x07,  /* Parallel Port 1          */
    RTC_IRQ         = IRQ_EXTERNAL + 0x08,  /* Real-time clock          */
    OPEN1_IRQ       = IRQ_EXTERNAL + 0x09,  /* Generic                  */
    OPEN2_IRQ       = IRQ_EXTERNAL + 0x0a,  /* Generic                  */
    OPEN3_IRQ       = IRQ_EXTERNAL + 0x0b,  /* Generic                  */
    PSMSE_IRQ       = IRQ_EXTERNAL + 0x0c,  /* PS2 Mouse                */
    FPU_IRQ         = IRQ_EXTERNAL + 0x0d,  /* Coprocessor              */
    ATA1_IRQ        = IRQ_EXTERNAL + 0x0e,  /* Primary hard drive       */
    ATA2_IRQ        = IRQ_EXTERNAL + 0x0f,  /* Secondary hard drive     */
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

#endif  /* __ASSEMBLY__ */

#include <asm-generic/irq.h>

#endif  /* _ASM_X86_IRQ_H_ */
