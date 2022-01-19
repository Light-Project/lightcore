/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_SETJMP_H_
#define _ASM_CSKY_SETJMP_H_

#define JMP_BUF_LEN 36
typedef char jmp_buf[JMP_BUF_LEN];

extern int __returns_twice setjmp(jmp_buf env);
extern void __noreturn longjmp(jmp_buf env, int val);

#endif /* _ASM_CSKY_SETJMP_H_ */
