/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_BOOTPARAM_H_
#define _ASM_X86_BOOTPARAM_H_

#include <asm/e820.h>

struct bootparam {
    struct e820_table e820_table;
};

#endif  /* _ASM_X86_BOOTPARAM_H_ */
