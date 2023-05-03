/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PANIC_H_
#define _PANIC_H_

#include <notifier.h>

extern int panic_timeout;
extern unsigned int panic_timestep;

DECLARE_NOTIFIER_SPIN_HEAD(panic_notifier);
DECLARE_NOTIFIER_SPIN_HEAD(panic_blink);

extern __printf(1, 0) __noreturn __cold void vpanic(const char *fmt, va_list args);
extern __printf(1, 2) __noreturn __cold void panic(const char *fmt, ...);

#endif /* _PANIC_H_ */
