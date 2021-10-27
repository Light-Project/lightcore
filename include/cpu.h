/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CPU_H_
#define _CPU_H_

#include <types.h>

#ifdef CONFIG_SMP

#else  /* !CONFIG_SMP */

#define cpu_for_each(cpu) for(cpu = 0; cpu < 1; ++cpu)
#define cpu_for_each_online(cpu) for_each_cpu(cpu)
#define cpu_for_each_present(cpu) for_each_cpu(cpu)
#define cpu_for_each_possible(cpu) for_each_cpu(cpu)

#endif  /* CONFIG_SMP */
#endif  /* _CPU_H_ */
