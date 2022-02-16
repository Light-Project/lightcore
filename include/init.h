/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _INIT_H_
#define _INIT_H_

#include <types.h>

#define boot_args_size 1024

extern char boot_args[boot_args_size];
extern struct sched_task init_task;

extern void __init arch_setup(void);
extern int __init __noreturn user_init(void *arg);
extern state __init vfl_init_root(void);

#endif  /* _INIT_H_ */
