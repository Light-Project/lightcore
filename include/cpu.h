/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CPU_H_
#define _CPU_H_

#include <types.h>

#ifdef CONFIG_SMP

#else  /* !CONFIG_SMP */

#define for_each_cpu(cpu) for(cpu = 0; cpu < 1; ++cpu)
#define for_each_online_cpu(cpu) for_each_cpu(cpu)
#define for_each_present_cpu(cpu) for_each_cpu(cpu)
#define for_each_possible_cpu(cpu) for_each_cpu(cpu)

#endif  /* CONFIG_SMP */
#endif  /* _CPU_H_ */
