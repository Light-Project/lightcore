/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_SCHED_H_
#define _LIGHTCORE_SCHED_H_

enum clone_flags {
    CLONE_VM    = 0x00000001, /* shared virtmem between processes */
    CLONE_FS    = 0x00000002, /* shared filesystem between processes */
    CLONE_FILES = 0x00000004, /* shared open files between processes */
};

#endif  /* _LIGHTCORE_SCHED_H_ */
