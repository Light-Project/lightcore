/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _INIT_INIT_H_
#define _INIT_INIT_H_

#define boot_args_size 1024
extern char boot_args[boot_args_size];

void arch_setup(void);

#endif
