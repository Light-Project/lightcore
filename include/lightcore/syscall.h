/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_SYSCALL_H_
#define _LIGHTCORE_SYSCALL_H_

#include <lightcore/asm/syscall.h>

enum syscall_number {
    syscall_restart         = 0,
    syscall_exit            = 1,
    syscall_fork            = 2,
    syscall_read            = 3,
    syscall_write           = 4,
    syscall_open            = 5,
    syscall_close           = 6,
    syscall_waitpid         = 7,
    syscall_creat           = 8,
    syscall_link            = 9,
    syscall_unlink          = 10,
    syscall_clone           = 120,
    syscall_sched_yield     = 158,
    syscall_vfork           = 190,
    syscall_msgget          = 399,
    syscall_msgsnd          = 400,
    syscall_msgrcv          = 401,
    syscall_msgctl          = 402,
    syscall_selftest        = 800,
};

#define GENERIC_SYSCALL_OP(                         \
    name, callnr, ret_type, type1,                  \
    type2, type3, type4, type5, type6               \
)                                                   \
static __always_inline ret_type                     \
generic_syscall_op_##name (                         \
    type1 arg1, type2 arg2, type3 arg3,             \
    type4 arg4, type5 arg5, type6 arg6              \
)                                                   \
{                                                   \
    return (ret_type) syscall (                     \
            callnr,                                 \
            (long)arg1, (long)arg2, (long)arg3,     \
            (long)arg4, (long)arg5, (long)arg6      \
    );                                              \
}

#define GENERIC_SYSCALL_ARG6(                       \
    name, callnr, ret_type,                         \
    type1, type2, type3, type4, type5, type6        \
)                                                   \
GENERIC_SYSCALL_OP(                                 \
    name, callnr, ret_type,                         \
    type1, type2, type3, type4, type5, type6        \
)                                                   \
static __always_inline ret_type                     \
generic_syscall_##name (                            \
    type1 arg1, type2 arg2, type3 arg3,             \
    type4 arg4, type5 arg5, type6 arg6              \
)                                                   \
{                                                   \
    return generic_syscall_op_##name (              \
        arg1, arg2, arg3, arg4, arg5, arg6          \
    );                                              \
}

#define GENERIC_SYSCALL_ARG5(                       \
    name, callnr, ret_type,                         \
    type1, type2, type3, type4, type5               \
)                                                   \
GENERIC_SYSCALL_OP(                                 \
    name, callnr, ret_type,                         \
    type1, type2, type3, type4, type5, int          \
)                                                   \
static __always_inline ret_type                     \
generic_syscall_arg5_##name (                       \
    type1 arg1, type2 arg2, type3 arg3,             \
    type4 arg4, type5 arg5                          \
)                                                   \
{                                                   \
    return generic_syscall_op_##name (              \
        arg1, arg2, arg3, arg4, arg5, 0             \
    );                                              \
}

#define GENERIC_SYSCALL_ARG4(                       \
    name, callnr, ret_type,                         \
    type1, type2, type3, type4                      \
)                                                   \
GENERIC_SYSCALL_OP(                                 \
    name, callnr, ret_type,                         \
    type1, type2, type3, type4, int, int            \
)                                                   \
static __always_inline ret_type                     \
generic_syscall_##name(                             \
    type1 arg1, type2 arg2, type3 arg3,             \
    type4 arg4                                      \
)                                                   \
{                                                   \
    return generic_syscall_op_##name (              \
        arg1, arg2, arg3, arg4, 0, 0                \
    );                                              \
}

#define GENERIC_SYSCALL_ARG3(                       \
    name, callnr, ret_type,                         \
    type1, type2, type3                             \
)                                                   \
GENERIC_SYSCALL_OP(                                 \
    name, callnr, ret_type, type1,                  \
    type2, type3, int, int, int                     \
)                                                   \
static __always_inline ret_type                     \
generic_syscall_##name (                            \
    type1 arg1, type2 arg2, type3 arg3              \
)                                                   \
{                                                   \
    return generic_syscall_op_##name (              \
        arg1, arg2, arg3, 0, 0, 0                   \
    );                                              \
}

#define GENERIC_SYSCALL_ARG2(                       \
    name, callnr, ret_type,                         \
    type1, type2                                    \
)                                                   \
GENERIC_SYSCALL_OP(                                 \
    name, callnr, ret_type,                         \
    type1, type2, int, int, int, int                \
)                                                   \
static __always_inline ret_type                     \
generic_syscall_##name (                            \
    type1 arg1, type2 arg2                          \
)                                                   \
{                                                   \
    return generic_syscall_op_##name (              \
        arg1, arg2, 0, 0, 0, 0                      \
    );                                              \
}

#define GENERIC_SYSCALL_ARG1(                       \
    name, callnr, ret_type,                         \
    type1                                           \
)                                                   \
GENERIC_SYSCALL_OP(                                 \
    name, callnr, ret_type,                         \
    type1, int, int, int, int, int                  \
)                                                   \
static __always_inline ret_type                     \
generic_syscall_##name (                            \
    type1 arg1                                      \
)                                                   \
{                                                   \
    return generic_syscall_op_##name (              \
        arg1, 0, 0, 0, 0, 0                         \
    );                                              \
}

#define GENERIC_SYSCALL_ARG0(                       \
    name, callnr, ret_type                          \
)                                                   \
GENERIC_SYSCALL_OP(                                 \
    name, callnr, ret_type,                         \
    int, int, int, int, int, int                    \
)                                                   \
static __always_inline ret_type                     \
generic_syscall_##name (                            \
    void                                            \
)                                                   \
{                                                   \
    return generic_syscall_op_##name (              \
        0, 0, 0, 0, 0, 0                            \
    );                                              \
}

GENERIC_SYSCALL_ARG0(exit, syscall_exit, void)
#define lightcore_exit() generic_syscall_exit()

GENERIC_SYSCALL_ARG3(fork, syscall_fork, long, int, void *, unsigned long)
#define lightcore_fork(fd, buf, count) generic_syscall_fork(fd, buf, count)

GENERIC_SYSCALL_ARG3(read, syscall_write, long, int, void *, unsigned long)
#define lightcore_read(fd, buf, count) generic_syscall_read(fd, buf, count)

GENERIC_SYSCALL_ARG3(write, syscall_write, long, int, void *, unsigned long)
#define lightcore_write(fd, buf, count) generic_syscall_write(fd, buf, count)

GENERIC_SYSCALL_ARG6(selftest, syscall_selftest, int, int, int, int, int, int, int)
#define lightcore_selftest(a1, a2, a3, a4, a5, a6) generic_syscall_selftest(a1, a2, a3, a4, a5, a6)

#endif /* _LIGHTCORE_SYSCALL_H_ */
