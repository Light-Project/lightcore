/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <kernel.h>
#include <irqflags.h>
#include <crash.h>
#include <printk.h>

#include <asm/traps.h>
#include <asm/idt.h>
#include <asm/bug.h>
#include <asm/backtrace.h>
#include <asm/unaligned.h>

static bool trap_crash(struct regs *regs)
{
    bool warn = false;

    if (regs->ip < CONFIG_PAGE_OFFSET)
        return false;

    if (unaligned_get_u16((void *)regs->ip) != BUG_VALUE)
        return false;

    if (regs->flags & EFLAGS_IF)
        irq_local_enable();

    if (crash_report(regs->ip, regs) == CRASH_TYPE_WARN) {
        regs->ip += BUG_LEN;
        warn = true;
    }

    if (regs->flags & EFLAGS_IF)
        irq_local_disable();

    return warn;
}

static bool kernel_trap(struct regs *regs, unsigned long error_code,
                        unsigned long vector, const char *type)
{
    struct extable_entry *extable;

    if (trap_v8086_mode(regs)) {
        if (vector < TRAP_UD) {
            return true;
        }
    } else if (trap_user_mode(regs)) {
        return true;
    } else { /* kernel mode */
        if ((extable = extable_find(regs->ip))) {
            extable_fixup(regs, extable);
            return true;
        }

        oops(regs, error_code, type);
    }

    return false;
}

static void generic_trap(struct regs *regs, unsigned long error_code,
                         unsigned long vector, const char *type)
{
    if (kernel_trap(regs, error_code, vector, type))
        return;
}

DEFINE_IDTENTRY_RAW(invalid_opcode)
{
    if (!trap_user_mode(regs) && trap_crash(regs))
        return;

    generic_trap(regs, 0, TRAP_UD, "invalid opcode");
}

DEFINE_IDTENTRY_RAW(breakpoint)
{

}

DEFINE_IDTENTRY_NORMAL(spurious_interrupt)
{
    /* Do nothing */
}

DEFINE_IDTENTRY_NORMAL(overflow)
{
    generic_trap(regs, 0, TRAP_OF, "overflow");
}

DEFINE_IDTENTRY_NORMAL(divide_error)
{
    generic_trap(regs, 0, TRAP_DE, "divide error");
}

DEFINE_IDTENTRY_NORMAL(bounds)
{
    generic_trap(regs, 0, TRAP_BR, "bounds");
}

DEFINE_IDTENTRY_NORMAL(device_not_available)
{
    generic_trap(regs, 0, TRAP_NM, "device not available");
}

DEFINE_IDTENTRY_NORMAL(coprocessor_error)
{
    generic_trap(regs, 0, TRAP_MF, "coprocessor error");
}

DEFINE_IDTENTRY_NORMAL(simd_error)
{
    generic_trap(regs, 0, TRAP_XM, "simd error");
}

DEFINE_IDTENTRY_ERRORCODE(invalid_tss)
{
    generic_trap(regs, error_code, TRAP_TS, "invalid tss");
}

DEFINE_IDTENTRY_ERRORCODE(segment_not_present)
{
    generic_trap(regs, error_code, TRAP_NP, "segment not present");
}

DEFINE_IDTENTRY_ERRORCODE(stack_segment)
{
    generic_trap(regs, error_code, TRAP_SS, "stack segment");
}

DEFINE_IDTENTRY_ERRORCODE(general_protection)
{
    generic_trap(regs, error_code, TRAP_GP, "general protection");
}

DEFINE_IDTENTRY_ERRORCODE(alignment_check)
{
    generic_trap(regs, error_code, TRAP_AC, "alignment check");
}

DEFINE_IDTENTRY_RAW_ERRORCODE(page_fault)
{
    generic_trap(regs, error_code, TRAP_OF, "page fault");
}

DEFINE_IDTENTRY_DF(double_fault)
{

}

DEFINE_IDTENTRY_DEBUG(debug)
{

}
