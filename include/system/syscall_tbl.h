/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SYSTEM_SYSCALL_TABLE_H_
#define _SYSTEM_SYSCALL_TABLE_H_

/**
 * Basics syscall
 * Syscall number range: 0-200
 * 
 * This part is compatible other system
 * 
 */
#define syscall_restart     0       /*  */
#define syscall_exit        1       /*  */
#define syscall_fork        2       /*  */
#define syscall_read        3       /*  */
#define syscall_write       4       /*  */
#define syscall_open        5       /*  */
#define syscall_close       6       /*  */
#define syscall_waitpid     7       /*  */
#define syscall_creat       8       /*  */
#define syscall_link        9       /*  */

#define SYSCALL_NR_MAX  500

#endif
