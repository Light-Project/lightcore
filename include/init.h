/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _INIT_H_
#define _INIT_H_

#include <types.h>

#define BOOT_PARAM_SIZE 512
extern char boot_args[BOOT_PARAM_SIZE];

extern void __init arch_setup(void);
extern state __init mount_rootfs(void);
extern int __noreturn user_init(void *arg);

extern void initcalls(void);
extern void __init ctors_initcall(void);
extern void __init bootargs_init(const char *cmd);

#endif  /* _INIT_H_ */
