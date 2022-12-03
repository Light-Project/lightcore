/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STACKPROTECTOR_H_
#define _STACKPROTECTOR_H_

#include <asm/stackprotector.h>

extern unsigned long random_canary(void);
extern void __weak __visible __stack_chk_fail(void);

#endif  /* _STACKPROTECTOR_H_ */
