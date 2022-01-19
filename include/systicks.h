/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SYSTICKS_H_
#define _SYSTICKS_H_

struct clockevent_device;

extern void systick_setup(struct clockevent_device *cdev);

#endif  /* _SYSTICKS_H_ */
