/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TASK_H_
#define _TASK_H_

#include <types.h>
#include <list.h>
#include <time.h>

#include <asm/regs.h>

struct task {
    pid_t pid;

    struct regs *stack;

    struct task *parent;
    struct list_head child;
    struct list_head sibling;

    struct timespec utime;
};

#endif  /* _TASK_H_ */
