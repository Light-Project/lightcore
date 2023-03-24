/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_SCHED_H_
#define _LIGHTCORE_SCHED_H_

enum clone_flags {
    CLONE_SIGNAL    = 0x000000ff,   /* exit signal mask */
    CLONE_VM        = 0x00000100,   /* shared virtmem between processes */
    CLONE_FS        = 0x00000200,   /* shared filesystem between processes */
    CLONE_FILES     = 0x00000400,   /* shared open files between processes */
    CLONE_VFORK     = 0x00004000,   /* wake up the child process first */
};

#endif /* _LIGHTCORE_SCHED_H_ */
