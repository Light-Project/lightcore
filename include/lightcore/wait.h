/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_WAIT_H_
#define _LIGHTCORE_WAIT_H_

#define WNOHANG     0x00000001  /* Do not hang if no status is available; return immediately. */
#define WUNTRACED   0x00000002  /* Report status of stopped child process. */
#define WSTOPPED    WUNTRACED   /* Status is returned for any child that has stopped upon receipt of a signal. */
#define WEXITED     0x00000004  /* Wait for processes that have exited. */
#define WCONTINUED	0x00000008  /* Status is returned for any child that was stopped and has been continued. */
#define WNOWAIT		0x01000000  /* Keep the process whose status is returned in infop in a waitable state. */
#define WAIT_ALLOW  (WNOHANG | WUNTRACED | WSTOPPED | WEXITED | WCONTINUED | WNOWAIT)

#define P_ALL		0
#define P_PID		1
#define P_PGID		2
#define P_PIDFD		3

#endif  /* _LIGHTCORE_WAIT_H_ */
