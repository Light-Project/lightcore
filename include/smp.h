/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SMP_H_
#define _SMP_H_

#include <cpu.h>
#include <asm/smp.h>

#ifdef CONFIG_SMP
# define smp_processor_id()
#else
# define smp_processor_id()     0
#endif

#endif  /* _SMP_H_ */
