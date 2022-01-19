/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _COMPILER_SECTIONS_H_
#define _COMPILER_SECTIONS_H_

#define __init  __cold  __section(".init.text")
#define __initdata      __section(".init.data")
#define __initconst     __section(".init.rodata")
#define __exitdata      __section(".exit.data")
#define __exit_call     __section(".exit.exitcall")
#define __sched         __section(".text.sched")
#define __read_mostly   __section(".data.readmostly")

#endif   /* _COMPILER_SECTIONS_H_ */
