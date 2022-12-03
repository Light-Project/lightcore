/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PERCPU_H
#define _ASM_X86_PERCPU_H

#ifdef CONFIG_X86_64
# define arch_percpu_seg gs
#else
# define arch_percpu_seg fs
#endif

#ifdef __ASSEMBLY__

#ifdef CONFIG_SMP
# define PERCPU_VAR(var) %__percpu_seg:var
#else /* !CONFIG_SMP */
# define PERCPU_VAR(var) var
#endif /* CONFIG_SMP */

#ifdef CONFIG_X86_64_SMP
# define INIT_PERCPU_VAR(var) init_per_cpu_##var
#else
# define INIT_PERCPU_VAR(var) var
#endif

#endif  /* __ASSEMBLY__ */

#include <asm-generic/percpu.h>

#endif  /* _ASM_X86_PERCPU_H */
