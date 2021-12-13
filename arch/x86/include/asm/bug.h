/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_BUG_H_
#define _ASM_X86_BUG_H_

#define BUG_OPCODE  ".byte 0x0f, 0x0b"
#define BUG_VALUE   0x0b0f
#define BUG_LEN     2

#define WARN() ({               \
    asm volatile(BUG_OPCODE);   \
})

#define BUG() ({                \
    asm volatile(BUG_OPCODE);   \
    unreachable();              \
})

#include <asm-generic/bug.h>

#endif  /* _ASM_X86_BUG_H_ */
