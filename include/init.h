/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _INIT_H_
#define _INIT_H_

#include <types.h>

#define BOOT_PARAM_SIZE 512
extern char boot_args[BOOT_PARAM_SIZE];

extern __init void arch_setup(void);
extern __init state mount_rootfs(void);
extern __noreturn int user_init(void *arg);

extern __init void initcalls(void);
extern __init void ctors_init(void);
extern __init void earlyargs_init(const char *cmd);
extern __init void bootargs_init(const char *cmd);

#endif /* _INIT_H_ */
