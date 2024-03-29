/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _INIT_H_
#define _INIT_H_

#include <types.h>
#include <completion.h>

#define BOOT_PARAM_SIZE 512
extern char boot_args[BOOT_PARAM_SIZE];
extern struct completion kernel_complete;

extern __init void initcalls(void);
extern __init void ctors_init(void);
extern __init void earlyargs_init(const char *cmd);
extern __init void bootargs_init(const char *cmd);

extern __init void arch_setup(void);
extern __init state mount_rootfs(void);
extern __noreturn int kernel_init(void *arg);

#endif /* _INIT_H_ */
