/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_SYSCALL_H_
#define _LIGHTCORE_SYSCALL_H_

#include <lightcore/asm/syscall.h>

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
    SYSCALL_NR_CLONE    = 120,
    SYSCALL_NR_VFORK    = 190,
    SYSCALL_NR_MSGGET   = 399,
    SYSCALL_NR_MSGSND   = 400,
    SYSCALL_NR_MSGRCV   = 401,
    SYSCALL_NR_MSGCTL   = 402,
    SYSCALL_NR_MAX      = 512,
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

GENERIC_SYSCALL_ARG0(exit, SYSCALL_NR_EXIT, void)
#define lightcore_exit() generic_syscall_exit()

GENERIC_SYSCALL_ARG3(fork, SYSCALL_NR_FORK, long, int, void *, unsigned long)
#define lightcore_fork(fd, buf, count) generic_syscall_fork(fd, buf, count)

GENERIC_SYSCALL_ARG3(read, SYSCALL_NR_WRITE, long, int, void *, unsigned long)
#define lightcore_read(fd, buf, count) generic_syscall_read(fd, buf, count)

GENERIC_SYSCALL_ARG3(write, SYSCALL_NR_WRITE, long, int, void *, unsigned long)
#define lightcore_write(fd, buf, count) generic_syscall_write(fd, buf, count)

#endif  /* _LIGHTCORE_SYSCALL_H_ */
