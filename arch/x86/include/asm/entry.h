/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_ENTRY_H_ 
#define _ASM_X86_ENTRY_H_

#include <linkage.h>

asmlinkage void entry_divide_error(void);
asmlinkage void entry_nmi(void);
asmlinkage void entry_syscall(void);
asmlinkage void entry_general_protection(void);

asmlinkage void entry_undefine(void);


#endif /* _ASM_X86_ENTRY_H_ */
