/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SLEEP_H_
#define _SLEEP_H_

#include <types.h>
#include <asm/delay.h>

extern intmax_t tick_time;

void msleep(unsigned int ms);

#endif /* _TIMER_H_ */
