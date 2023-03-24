/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ENTRY_H_
#define _ENTRY_H_

#include <asm/regs.h>
#include <asm/traps.h>

extern noinline void entry_irqhandle_enter_form_user(struct regs *regs);
extern noinline void entry_irqhandle_exit_to_user(struct regs *regs);
extern noinline void entry_irqhandle_enter(struct regs *regs);
extern noinline void entry_irqhandle_exit(struct regs *regs);

#endif /* _ENTRY_H_ */
