/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SETJMP_H_
#define _SETJMP_H_

#include <asm/setjmp.h>

/*
 * Store the calling environment in ENV, not saving the signal mask.
 * Return 0.
 */
extern int __returns_twice setjmp(jmp_buf *env);

/*
 * Jump to the environment saved in ENV, making the
 * 'setjmp' call there return VAL, or 1 if VAL is 0.
 */
extern void __noreturn longjmp(jmp_buf *env, int val);

#endif  /* _LIGHTCORE_SETJMP_H_ */
