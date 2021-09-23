/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>

    .text
SYM_CODE_START(strcmp)
    ldrb    v1, [a1], #1
    ldrb    v2, [a2], #1
    cmp     v1, #1
    cmpcs   v1, v2
    beq     strcmp
    sub     r0, v1, v2
    mov     pc, lr
SYM_FUNC_END(strcmp)
