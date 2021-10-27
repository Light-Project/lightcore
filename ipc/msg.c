/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <list.h>
#include <ipc.h>
#include <syscall.h>
#include <lightcore/msg.h>

struct msg_send {
    struct task *task;
    struct list_head list;
};

struct msg_recv {
    struct task *task;
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

static long syscall_msgget(key_t key, int flags)
{
    return ipc_msgget(key, flags);
}

static long syscall_msgctl()
{

}

static long syscall_msgsnd(int msqid, struct msgbuf *msgp,
                            size_t msgsz, int msgflg)
{
    return ipc_msgsnd(msqid, msgp, msgsz, msgflg);
}

static long syscall_msgrcv(int msqid, struct msgbuf *msgp,
                            size_t msgsz, long msgtyp, int msgflg)
{
    return ipc_msgrcv(msqid, msgp, msgsz, msgtyp, msgflg);
}

SYSCALL_ENTRY(SYSCALL_NR_MSGGET, syscall_msgget, 2);
SYSCALL_ENTRY(SYSCALL_NR_MSGCTL, syscall_msgctl, 3);
SYSCALL_ENTRY(SYSCALL_NR_MSGSND, syscall_msgsnd, 4);
SYSCALL_ENTRY(SYSCALL_NR_MSGRCV, syscall_msgrcv, 5);
