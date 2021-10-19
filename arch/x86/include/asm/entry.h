/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_ENTRY_H_
#define _ASM_X86_ENTRY_H_

#include <linkage.h>

asmlinkage void entry_divide_error(void);
asmlinkage void entry_nmi_interrupt(void);
asmlinkage void entry_breakpoint(void);
asmlinkage void entry_bounds(void);
asmlinkage void entry_invalid_opcode(void);
asmlinkage void entry_device_not_available(void);
asmlinkage void entry_invalid_tss(void);
asmlinkage void entry_segment_not_present(void);
asmlinkage void entry_stack_segment(void);
asmlinkage void entry_general_protection(void);
asmlinkage void entry_spurious_interrupt(void);
asmlinkage void entry_coprocessor_error(void);
asmlinkage void entry_alignment_check(void);
asmlinkage void entry_simd_error(void);
asmlinkage void entry_overflow(void);
asmlinkage void entry_syscall_80(void);
asmlinkage void entry_double_fault(void);
asmlinkage void entry_debug(void);

asmlinkage void entry_generic_interrupt(void);
asmlinkage void entry_switch_stack(void *prev, void *next);

#endif /* _ASM_X86_ENTRY_H_ */
