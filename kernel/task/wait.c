/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <task.h>
#include <wait.h>
#include <syscall.h>
#include <export.h>

struct task_wait_args {
    enum pid_type type;
    struct pid *pid;
    int wait_flags;
};

static task_wait_zombie(struct task_wait_args *args, struct sched_task *task)
{

}

static task_wait_stopped(struct task_wait_args *args, struct sched_task *task)
{



}

static task_wait_dead(struct task_wait_args *args, struct sched_task *task)
{
    if (task->exit == SCHED_TASK_DEAD)
        return -ENOERR;

}

static state task_wait_child(struct task_wait_args *args, struct sched_task *task)
{
    struct sched_task *walk;
    state retval = 0;

    list_for_each_entry(walk, &task->child, sibling) {
        if ((retval = task_wait_dead(args, walk)))
            break;
    }

    return retval;
}

static state task_wait_ptrace(struct task_wait_args *args, struct sched_task *task)
{
    struct sched_task *walk;
    state retval = 0;

    list_for_each_entry(walk, &task->ptraced, ptrace_sibling) {
        if ((retval = task_wait_dead(args, walk)))
            break;
    }

    return retval;
}

static state task_wait(struct task_wait_args *args)
{
    state retval;

retry:

    current_set_state(SCHED_TASK_INTERRUPTIBLE);

    if (args->type == PID_TYPE_PID) {


    } else {



    }

notask:
    if ((args->wait_flags & WNOHANG) && !retval) {


    }

finish:
    current_set_state(SCHED_TASK_RUNNING);
    return retval;
}

state kernel_waitpid(pid_t upid, int which, int options)
{
    struct task_wait_args args = {};
    state retval;

    if (options & ~WAIT_ALLOW)
        return -EINVAL;

    if (!(options & (WEXITED | WSTOPPED | WCONTINUED)))
        return -EINVAL;

    switch (which) {
        case P_ALL:
            args.type = PID_TYPE_MAX;
            break;

        case P_PID:
            args.type = PID_TYPE_PID;
            break;

        case P_PGID:
            args.type = PID_TYPE_PGID;
            break;

        case P_PIDFD:
            args.type = PID_TYPE_PID;
            break;

        default:
            return -EINVAL;
    }

    retval = task_wait(&args);

    pid_put(args.pid);
    return retval;
}

long syscall_waitpid(pid_t pid, int *stat, int options)
{
    return kernel_wait(pid, stat);
}

EXPORT_SYMBOL(kernel_wait);
SYSCALL_ENTRY(SYSCALL_NR_WAITPID, syscall_waitpid, 3);
