/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>

SYM_CODE_START(memcmp)
    /* if(len == 0) return 0 */
    cmp     a3, #0
    moveq   a1, #0
    moveq   pc, lr

    /* subtract one to have the index of the last character to check */
    sub     r2, r2, #1

    /* ip == last src address to compare */
    add     ip, r0, r2

1:
    ldrb    r2, [r0], #1
    ldrb    r3, [r1], #1
    cmp     ip, r0
    cmpcs   r2, r3
    beq     1b
    sub     r0, r2, r3

    mov		pc, lr
SYM_FUNC_END(memcmp)
