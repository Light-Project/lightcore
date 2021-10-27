/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_MSG_H_
#define _LIGHTCORE_MSG_H_

struct msgbuf {
    long mtype;
    char mtext[1];
};

#endif  /* _LIGHTCORE_MSG_H_ */
