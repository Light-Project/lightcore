/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IDT_H_
#define _ASM_X86_IDT_H_

#include <linkage.h>
#include <system/irq.h>
#include <arch/x86/segment.h>
#include <arch/x86/interrupt.h>

#define IDT_SET(table, num, offset, seg, _type, _d, _dpl)           \
        do{                                                         \
        table[num].offsetl  = (uint32_t)(offset) & 0xffff;          \
        table[num].offseth  = ((uint32_t)(offset) >> 16) & 0xffff;  \
        table[num].segment  = (seg) & 0xffff;                       \
        table[num].type     = (_type) & 0x07;                       \
        table[num].d        = (_d) & 0x01;                          \
        table[num].dpl      = (_dpl) & 0x03;                        \
        table[num].p        = 0x01;                                 \
        table[num].RES0     = 0x00;                                 \
        table[num].RES1     = 0x00;                                 \
        }while(0)           

#define IDT_SET_DEFAULTS_INTERRUPT(table, num, func)        \
    IDT_SET(idt_entry, num, func, GDT_ENTRY_BOOT_CS_BASE,   \
            IDT_TYPE_INTERRUPT, IDT_D_32, IDT_DPL_RING0)

#define IDT_SET_KERNEL(table, num, func)                    \
    IDT_SET(idt_entry, num, func, GDT_ENTRY_KERNEL_CS_BASE, \
            IDT_TYPE_INTERRUPT, IDT_D_32, IDT_DPL_RING0)

#define IDT_DATA_ENTRY(vec, fn)             \
        .vector = (vec),                    \
        .func = (fn),                       \
        .segment = GDT_ENTRY_BOOT_CS_BASE,
        // .type = IDT_TYPE_INTERRUPT,
        // .d = IDT_D_32, .dpl = IDT_DPL_RING0

struct idt_data {
	irqnr_t vector;         /* Vector number */
    void (*func)(void);     /* Vector fun */
	uint16_t segment;       /* Vector segment */
};

static inline void idt_load(struct idt_table *idt)
{
    asm volatile("lidtl (%0)"::"Na"(idt));
}

#define IDTENTRY_IRQ(func) \

#define SYSCALL_TRAP 0x80

asmlinkage void entry_divide_fault(void);

void arch_idt_setup(void);

#endif /* _ASM_X86_IDT_H_ */
