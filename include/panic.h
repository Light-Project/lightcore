/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PANIC_H_
#define _PANIC_H_

#include <asm/bug.h>

extern __printf(1, 2) __cold
void panic(const char* fmt, ...);

#endif  /* _PANIC_H_ */
