/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_ALTERNATIVE_H_
#define _ASM_X86_ALTERNATIVE_H_

#ifndef __ASSEMBLY__

#include <types.h>
#include <stddef.h>

#ifdef CONFIG_SMP
# define LOCK_PREFIX_HERE               \
    ".pushsection .smp_locks,\"a\"\n"   \
    ".balign 4\n"                       \
    ".long 671f - .\n" /* offset */     \
    ".popsection\n"                     \
    "671:"
# define LOCK_PREFIX LOCK_PREFIX_HERE "\n\tlock; "
#else
# define LOCK_PREFIX_HERE ""
# define LOCK_PREFIX ""
#endif

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_ALTERNATIVE_H_ */
