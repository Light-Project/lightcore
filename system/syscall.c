/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <types.h>
#include <system/syscall.h>
#include <system/sched.h>

pid_t sys_getpid(void)
{
    
}

static void* syscall_table[SYSCALL_NR_MAX] = {
    [syscall_write] = sys_getpid,
    
};
