/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_DOUBLEFAULT_H_
#define _ASM_X86_DOUBLEFAULT_H_

#ifndef CONFIG_X86_64
void doublefault_init(void);
#else
static inline void doublefault_init(void) { }
#endif

#endif /* _ASM_X86_DOUBLEFAULT_H_ */
