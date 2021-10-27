/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <types.h>
#include <state.h>
#include <initcall.h>

enum syscall_number {
    SYSCALL_NR_RESTART  = 0,
    SYSCALL_NR_EXIT     = 1,
    SYSCALL_NR_FORK     = 2,
    SYSCALL_NR_READ     = 3,
    SYSCALL_NR_WRITE    = 4,
    SYSCALL_NR_OPEN     = 5,
    SYSCALL_NR_CLOSE    = 6,
    SYSCALL_NR_WAITPID  = 7,
    SYSCALL_NR_CREAT    = 8,
    SYSCALL_NR_LINK     = 9,
    SYSCALL_NR_MSGGET   = 399,
    SYSCALL_NR_MSGSND   = 400,
    SYSCALL_NR_MSGRCV   = 401,
    SYSCALL_NR_MSGCTL   = 402,
    SYSCALL_NR_MAX      = 512,
};

struct syscall_entry {
    const char *name;
    state (*entry)(void);
    unsigned int args;
};

state syscall_register(unsigned int call_num, struct syscall_entry *syscall);
void syscall_unregister(unsigned int call_num);

#define SYSCALL_ENTRY(call_num, fun, arg_nr)                \
    static struct syscall_entry  entry_##fun = {            \
        .name = __stringify(fun), .args = arg_nr,           \
    };                                                      \
    static state init_##fun(void)                           \
    {                                                       \
        return syscall_register(call_num, &entry_##fun);    \
    }                                                       \
    core_initcall(init_##fun)

#endif  /* _SYSCALL_H_ */
