/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _POWER_H_
#define _POWER_H_

extern __noreturn void kernel_restart(void);
extern __noreturn void kernel_halt(void);
extern __noreturn void kernel_poweroff(void);

#endif  /* _POWER_H_ */
