/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _EXTABLE_H_
#define _EXTABLE_H_

#include <asm/extable.h>

extern struct extable_entry *extable_find(uintptr_t addr);
extern void __init extable_init(void);

#endif /* _EXTABLE_H_ */
