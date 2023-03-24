/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STATE_H_
#define _STATE_H_

#include <types.h>
#include <asm/errno.h>

#define ERESTARTSYS             512     /* restart if system call.. */
#define ERESTARTNOINTR          513     /* restart if no interrupt.. */
#define ERESTARTNOHAND          514     /* restart if no handler.. */
#define ENOIOCTLCMD             515     /* No ioctl command */
#define ERESTART_RESTARTBLOCK   516     /* restart by calling sys_restart_syscall */
#define EPROBE_DEFER            517     /* Driver requests probe retry */
#define EOPENSTALE              518     /* open found a stale dentry */
#define ENOPARAM                519     /* Parameter not supported */

#define EBADHANDLE              521     /* Illegal NFS file handle */
#define ENOTSYNC                522     /* Update synchronization mismatch */
#define EBADCOOKIE              523     /* Cookie is stale */
#define ENOTSUPP                524     /* Operation is not supported */
#define ETOOSMALL               525     /* Buffer or request is too small */
#define ESERVERFAULT            526     /* An untranslatable error occurred */
#define EBADTYPE                527     /* Type not supported by server */
#define EJUKEBOX                528     /* Request initiated, but will not complete before timeout */
#define EIOCBQUEUED             529     /* iocb queued, will get completion event */
#define ERECALLCONFLICT         530     /* conflict with recalled state */
#define ENOGRACE                531     /* NFS file lock reclaim refused */

#define ERRNO_MAX               4096

#define errno (*sched_current_errno())
extern state *sched_current_errno(void);

#endif /* _STATE_H_ */
