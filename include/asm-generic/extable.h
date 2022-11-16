/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_EXTABLE_H_
#define _ASM_GENERIC_EXTABLE_H_

#include <types.h>
#include <stddef.h>

#ifndef __ASSEMBLY__

struct extable_entry {
    uintptr_t addr;
    uintptr_t fixup;
    unsigned int data;
};

#ifndef EXTABLE_ASM
# define EXTABLE_ASM(addr, fixup, data)         \
    ".pushsection .data.extable, \"aw\"     \n" \
    "   .long   " __stringify(addr) "       \n" \
    "   .long   " __stringify(fixup) "      \n" \
    "   .int    " __stringify(data) "       \n" \
    ".popsection                            \n"
#endif

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_GENERIC_EXTABLE_H_ */
