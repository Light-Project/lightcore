/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_ENTRY_H_
#define _ASM_X86_ENTRY_H_

#include <linkage.h>

extern asmlinkage void entry_thread_return(void);
extern asmlinkage void entry_switch_stack(void **prev, void **next);

extern asmlinkage void entry_divide_error(void);
extern asmlinkage void entry_nmi_interrupt(void);
extern asmlinkage void entry_breakpoint(void);
extern asmlinkage void entry_bounds(void);
extern asmlinkage void entry_invalid_opcode(void);
extern asmlinkage void entry_device_not_available(void);
extern asmlinkage void entry_invalid_tss(void);
extern asmlinkage void entry_segment_not_present(void);
extern asmlinkage void entry_stack_segment(void);
extern asmlinkage void entry_general_protection(void);
extern asmlinkage void entry_spurious_interrupt(void);
extern asmlinkage void entry_coprocessor_error(void);
extern asmlinkage void entry_alignment_check(void);
extern asmlinkage void entry_simd_error(void);
extern asmlinkage void entry_overflow(void);
extern asmlinkage void entry_syscall_80(void);
extern asmlinkage void entry_double_fault(void);
extern asmlinkage void entry_debug(void);
extern asmlinkage void entry_generic_interrupt(void);

#endif /* _ASM_X86_ENTRY_H_ */
