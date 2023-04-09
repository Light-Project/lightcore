/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <bitfield.h>
#include <once.h>
#include <crash.h>
#include <extable.h>

static inline void
extable_handler_generic(struct regs *regs, struct extable_entry *extable)
{
    unsigned int flags = FIELD_GET(EXTABLE_FLAG_MASK, extable->data);

    if (flags & EXTABLE_FLAG_CLEAR_AX)
        regs->ax = 0;

    if (flags & EXTABLE_FLAG_CLEAR_DX)
        regs->dx = 0;

    regs->ip = extable->fixup;
}

static inline void
extable_handler_msr(struct regs *regs, struct extable_entry *extable,
                    bool safe, bool wrmsr, unsigned int reg)
{
    if (DO_ONCE_DONE(!safe && !wrmsr)) {
        pr_warn("unchecked msr access error: rdmsr form %#010x\n",
                (unsigned int)regs->cx);
    }

    if (DO_ONCE_DONE(!safe && wrmsr)) {
        pr_warn("unchecked msr access error: WDMSR to %#010x (tried to write %#010x%08x)\n",
                (unsigned int)regs->cx, (unsigned int)regs->dx,
                (unsigned int)regs->ax);
    }

    if (!wrmsr) {
        regs->ax = 0;
        regs->dx = 0;
    }

    extable_handler_generic(regs, extable);
}

void extable_fixup(struct regs *regs, struct extable_entry *extable)
{
    unsigned int type, reg;

    type = FIELD_GET(EXTABLE_TYPE_MASK, extable->data);
    reg = FIELD_GET(EXTABLE_REG_MASK, extable->data);

    switch (type) {
        case EXTABLE_TYPE_RDMSR:
            extable_handler_msr(regs, extable, false, false, reg);
            break;

        case EXTABLE_TYPE_WRMSR:
            extable_handler_msr(regs, extable, false, true, reg);
            break;

        case EXTABLE_TYPE_RDMSR_SAFE:
            extable_handler_msr(regs, extable, true, false, reg);
            break;

        case EXTABLE_TYPE_WRMSR_SAFE:
            extable_handler_msr(regs, extable, true, true, reg);
            break;

        default:
            BUG();
    }
}
