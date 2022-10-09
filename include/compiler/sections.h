/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _COMPILER_SECTIONS_H_
#define _COMPILER_SECTIONS_H_

#define __init __cold   __section(".init.text")
#define __initdata      __section(".init.data")
#define __initconst     __section(".init.rodata")

#define __exit __cold   __section(".exit.text")
#define __exitdata      __section(".exit.data")
#define __exitconst     __section(".exit.rodata")

#define __sched         __section(".text.sched")
#define __cpuidle       __section(".text.cpuidle")
#define __entry         __section(".text.entry")
#define __irq_entry     __section(".text.irqentry")
#define __softirq_entry __section(".text.softirqentry")
#define __read_mostly   __section(".data.readmostly")

#endif   /* _COMPILER_SECTIONS_H_ */
