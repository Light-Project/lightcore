/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIMERKEEPING_H_
#define _TIMERKEEPING_H_

#include <ktime.h>

extern volatile uint64_t ticktime;
extern void timer_update(unsigned long ticks);

#endif  /* _TIMERKEEPING_H_ */
