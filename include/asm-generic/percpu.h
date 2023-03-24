/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PERCPU_H_
#define _ASM_GENERIC_PERCPU_H_

#ifdef CONFIG_SMP
# define PER_CPU_SECTION(__sec) ".percpu" #__sec
#else
# define PER_CPU_SECTION(__sec) ".data"
#endif

#define DEFINE_PERCPU(type, name)   \
    __section(PER_CPU_SECTION(""))  \
    __used typeof(type) name

#define DECLARE_PERCPU(type, name)  \
    __section(PER_CPU_SECTION(""))  \
    extern typeof(type) name

#ifdef CONFIG_SMP
# define percpu_ptr(cpu, ptr)
#else
# define percpu_ptr(cpu, ptr) ({ (void)(cpu); (typeof(*(ptr))*) (ptr); })
#endif

#define percpu(cpu, name)  (*percpu_ptr(cpu, &(name)))

#ifdef CONFIG_SMP
# define thiscpu_ptr(name) percpu_ptr(smp_processor_id(), name)
#else
# define thiscpu_ptr(name) percpu_ptr(0, name)
#endif

#ifdef CONFIG_SMP
# define thiscpu(name) percpu(smp_processor_id(), name)
#else
# define thiscpu(name) percpu(0, name)
#endif

#endif /* _ASM_GENERIC_PERCPU_H_ */
