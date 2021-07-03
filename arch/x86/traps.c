/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <printk.h>
#include <system/power.h>

#include <asm/traps.h>
#include <asm/idt.h>
#include <asm/irq.h>
#include <asm/regs.h>


void show_regs(struct regs *regs)
{
    printk("EIP: %08lx EFLAGS: %08lx\n\r", regs->eip, regs->eflags);
	printk("EAX: %08lx EBX: %08lx ECX: %08lx EDX: %08lx\n\r",
		    regs->eax, regs->ebx, regs->ecx, regs->edx);
	printk("ESI: %08lx EDI: %08lx EBP: %08lx ESP: %08lx\n\r",
		    regs->esi, regs->edi, regs->ebp, regs->esp);
	printk("CS: %04x DS: %04x ES: %04x FS: %04x GS: %04x SS: %04x\n\r",
	        regs->cs, regs->ds, regs->es, regs->fs, regs->gs, regs->ss);
	printk("CR0: %08x CR2: %08x CR3: %08x CR4: %08x\n\r", 
			cr0_get(), cr2_get(), cr3_get(), cr4_get());
}

void trap_general_protection(struct regs *regs)
{
	
    show_regs(regs);
    power_halt();
}