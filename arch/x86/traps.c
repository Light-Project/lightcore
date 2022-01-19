/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <kernel.h>
#include <printk.h>

#include <asm/traps.h>
#include <asm/idt.h>
#include <asm/bug.h>
#include <asm/backtrace.h>

static bool trap_bug(struct regs *regs)
{
    if (regs->ip < CONFIG_PAGE_OFFSET)
        return false;

    if (*(uint16_t *)regs->ip != BUG_VALUE)
        return false;

    if (regs->flags & EFLAGS_IF)
        cpu_irq_enable();

    regs->ip += BUG_LEN;

    if (regs->flags & EFLAGS_IF)
        cpu_irq_disable();

    return true;
}

static void do_trap(struct regs *regs, unsigned int num, const char *type)
{
    if (!trap_user_mode(regs)) {
        oops(regs, type);
    }
}

asmlinkage __visible void trap_divide_error(struct regs *regs)
{
    do_trap(regs, TRAP_DE, "divide error");
}

asmlinkage __visible void trap_bounds(struct regs *regs)
{
    do_trap(regs, TRAP_BR, "bounds");
}

asmlinkage __visible void trap_device_not_available(struct regs *regs)
{
    do_trap(regs, TRAP_NM, "device not available");
}

asmlinkage __visible void trap_invalid_tss(struct regs *regs)
{
    do_trap(regs, TRAP_TS, "invalid tss");
}

asmlinkage __visible void trap_segment_not_present(struct regs *regs)
{
    do_trap(regs, TRAP_NP, "segment not present");
}

asmlinkage __visible void trap_stack_segment(struct regs *regs)
{
    do_trap(regs, TRAP_SS, "stack segment");
}

asmlinkage __visible void trap_coprocessor_error(struct regs *regs)
{
    do_trap(regs, TRAP_MF, "coprocessor error");
}

asmlinkage __visible void trap_alignment_check(struct regs *regs)
{
    do_trap(regs, TRAP_AC, "alignment check");
}

asmlinkage __visible void trap_simd_error(struct regs *regs)
{
    do_trap(regs, TRAP_XM, "simd error");
}

asmlinkage __visible void trap_general_protection(struct regs *regs)
{
    do_trap(regs, TRAP_GP, "general protection");
}

asmlinkage __visible void trap_overflow(struct regs *regs)
{
    do_trap(regs, TRAP_OF, "overflow");
}

asmlinkage __visible void trap_breakpoint(struct regs *regs)
{

}

asmlinkage __visible void trap_debug(struct regs *regs)
{

}

asmlinkage __visible void trap_invalid_opcode(struct regs *regs)
{
    if (!trap_user_mode(regs) && trap_bug(regs))
        return;
}

asmlinkage __visible void trap_nmi_interrupt(struct regs *regs)
{

}

asmlinkage __visible void trap_double_fault(struct regs *regs)
{
    do_trap(regs, TRAP_OF, "double fault");
}
