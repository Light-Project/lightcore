/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_ENTRY_H_
#define _ASM_CSKY_ENTRY_H_

#include <linkage.h>

asmlinkage void entry_page_fault(void);
asmlinkage void entry_generic_interrupt(void);

#endif /* _ASM_X86_ENTRY_H_ */
