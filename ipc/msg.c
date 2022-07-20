/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <ipc.h>
#include <syscall.h>
#include <lightcore/msg.h>

struct msg_send {
    struct sched_task *task;
    struct list_head list;
};

struct msg_recv {
    struct sched_task *task;
    struct list_head list;
};

struct msg_queue {
    struct list_head recv;
    struct list_head send;
};

static state msg_create(struct namespace_ipc *ns, int flags)
{
    struct msg_queue *msg;

    msg = kmalloc(sizeof(), GFP_KERNEL);


}

static struct ipc_type msg_ipc = {
    .create = msg_create,
};

long ipc_msgget(key_t key, int flags)
{

}

long ipc_msgctl(int msqid, int msgcmd)
{

}

long ipc_msgsnd(int msqid, struct msgbuf *msgp,
                size_t msgsz, int msgflg)
{
    for (;;) {

    }
}

long ipc_msgrcv(int msqid, struct msgbuf *msgp,
                size_t msgsz, long msgtyp, int msgflg)
{

}

DEFINE_SYSCALL2(msgget, key_t, key, int, flags)
{
    return ipc_msgget(key, flags);
}

DEFINE_SYSCALL4(msgsnd, int, msqid, struct msgbuf *, msgp,
                size_t, msgsz, int, msgflg)
{
    return ipc_msgsnd(msqid, msgp, msgsz, msgflg);
}

DEFINE_SYSCALL5(msgrcv, int, msqid, struct msgbuf0 *, msgp,
                size_t, msgsz, long, msgtyp, int, msgflg)
{
    return ipc_msgrcv(msqid, msgp, msgsz, msgtyp, msgflg);
}
