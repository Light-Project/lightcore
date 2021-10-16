/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIMER_H_
#define _TIMER_H_

#include <types.h>

extern volatile uint64_t ticktime;

void timer_tick(void);

#endif  /* _TIMER_H_ */
