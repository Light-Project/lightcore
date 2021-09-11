/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <types.h>
#include <linkage.h>

enum syscall_number {
    SYS_RESTART     = 0,
    SYS_EXIT        = 1,
    SYS_FORK        = 2,
    SYS_READ        = 3,
    SYS_WRITE       = 4,
    SYS_OPEN        = 5,
    SYS_CLOSE       = 6,
    SYS_WAITPID     = 7,
    SYS_CREAT       = 8,
    SYS_LINK        = 9,
    SYS_NR_MAX,
};

asmlinkage ssize_t sys_read(unsigned int fd, char *buf, size_t count);
asmlinkage ssize_t sys_write(unsigned int fd, const char *buf, size_t count);
asmlinkage int sys_open(const char *filename, int flags, umode_t mode);
asmlinkage int sys_close(unsigned int fd);

#endif  /* _SYSCALL_H_ */
