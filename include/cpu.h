/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CPU_H_
#define _CPU_H_

#include <types.h>

#ifdef CONFIG_SMP

#else /* !CONFIG_SMP */

static inline bool cpu_online(unsigned int cpu)
{
    return cpu == 0;
}

static inline bool cpu_possible(unsigned int cpu)
{
    return cpu_online(cpu);
}

static inline bool cpu_present(unsigned int cpu)
{
    return cpu_online(cpu);
}

static inline bool cpu_active(unsigned int cpu)
{
    return cpu_online(cpu);
}

# define cpu_for_each(cpu)              for (cpu = 0; cpu < 1; ++cpu)
# define cpu_for_each_online(cpu)       cpu_for_each(cpu)
# define cpu_for_each_present(cpu)      cpu_for_each(cpu)
# define cpu_for_each_possible(cpu)     cpu_for_each(cpu)
#endif /* CONFIG_SMP */

#define cpu_offline(cpu) unlikely(!cpu_online(cpu))

#endif /* _CPU_H_ */
