/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CPU_H_
#define _CPU_H_

#include <types.h>

#ifdef CONFIG_NRCPU_SHIFT
# define NRCPU_SHIFT CONFIG_NRCPU_SHIFT
#else
# define NRCPU_SHIFT 0
#endif

#define NRCPU_NONE   (unsigned int)(-1)
#define NRCPU_MAX    (1U << NRCPU_SHIFT)

#define cpu_for_each(cpu) for (cpu = 0; cpu < NRCPU_MAX; ++cpu)
#define cpu_for_each_reverse(cpu) for (cpu = NRCPU_MAX - 1; cpu != NRCPU_NONE; --cpu)

#define cpu_for_each_form(cpu) for (; cpu < NRCPU_MAX; ++cpu)
#define cpu_for_each_form_reverse(cpu) for (; cpu != NRCPU_NONE; --cpu)

#define cpu_for_each_continue(cpu) for (++cpu; cpu < NRCPU_MAX; ++cpu)
#define cpu_for_each_continue_reverse(cpu) for (--cpu; cpu != NRCPU_NONE; --cpu)

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

# define cpu_for_each_online(cpu) cpu_for_each(cpu)
# define cpu_for_each_present(cpu) cpu_for_each(cpu)
# define cpu_for_each_possible(cpu) cpu_for_each(cpu)

# define cpu_for_each_online_reverse(cpu) cpu_for_each(cpu)
# define cpu_for_each_present_reverse(cpu) cpu_for_each(cpu)
# define cpu_for_each_possible_reverse(cpu) cpu_for_each(cpu)

# define cpu_for_each_online_form(cpu) cpu_for_each_form(cpu)
# define cpu_for_each_present_form(cpu) cpu_for_each_form(cpu)
# define cpu_for_each_possible_form(cpu) cpu_for_each_form(cpu)

# define cpu_for_each_online_form_reverse(cpu) cpu_for_each_form_reverse(cpu)
# define cpu_for_each_present_form_reverse(cpu) cpu_for_each_form_reverse(cpu)
# define cpu_for_each_possible_form_reverse(cpu) cpu_for_each_form_reverse(cpu)

# define cpu_for_each_online_continue(cpu) cpu_for_each_continue(cpu)
# define cpu_for_each_present_continue(cpu) cpu_for_each_continue(cpu)
# define cpu_for_each_possible_continue(cpu) cpu_for_each_continue(cpu)

# define cpu_for_each_online_continue_reverse(cpu) cpu_for_each_continue_reverse(cpu)
# define cpu_for_each_present_continue_reverse(cpu) cpu_for_each_continue_reverse(cpu)
# define cpu_for_each_possible_continue_reverse(cpu) cpu_for_each_continue_reverse(cpu)

#endif /* CONFIG_SMP */

#define cpu_offline(cpu) unlikely(!cpu_online(cpu))

#endif /* _CPU_H_ */
