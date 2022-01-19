/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _INIT_H_
#define _INIT_H_

#define boot_args_size 1024

extern char boot_args[boot_args_size];
extern void __init arch_setup(void);
extern int __init __noreturn user_init(void *arg);

#endif  /* _INIT_H_ */
