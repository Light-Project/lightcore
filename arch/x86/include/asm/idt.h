/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IDT_H_
#define _ASM_X86_IDT_H_

#include <irqflags.h>
#include <arch/x86/segment.h>
#include <arch/x86/interrupt.h>

#define TRAP_SYSCALL 0x80

struct idt_data {
    irqnr_t vector;
    void *handle;
    uint16_t segment;
    struct idt_bits bits;
};

static inline void idt_set(struct idt_table *idt)
{
    asm volatile("lidt (%0)"::"Na"(idt));
}

void idt_int_gate(irqnr_t nr, void *handle);
void idt_setup(void);

#endif /* _ASM_X86_IDT_H_ */
