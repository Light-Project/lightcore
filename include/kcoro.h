/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KCORO_H_
#define _KCORO_H_

#include <types.h>
#include <list.h>
#include <kthread.h>

struct kcoro_work;
enum kcoro_retun;
typedef enum kcoro_retun (*kcoro_entry_t)(struct kcoro_work *, void *data);

enum kcoro_retun {
    KCORO_RET_EXITED,
    KCORO_RET_BLOCK,
};

struct kcoro_work {
    const char *name;
    struct kcoro_worker *worker;
    struct list_head list;
    kcoro_entry_t entry;
    void *data;
};

struct kcoro_worker {
    const char *name;
    struct list_head list;
    struct list_head work_list;
};

void kcoro_work_relax(struct kcoro_work *worker);
void kcoro_work_enqueue(struct kcoro_worker *worker, struct kcoro_work *work);
void kcoro_work_dequeue(struct kcoro_work *work);
struct kcoro_work *kcoro_work_create(kcoro_entry_t fun, void *data, const char *name, ...);
void kcoro_work_remove(struct kcoro_work *work);

void *kcoro_worker_loop(void *data);
struct kcoro_worker *kcoro_worker_best(void);
int kcoro_worker_wakeup(struct kcoro_worker *worker);
void kcoro_worker_suspend(struct kcoro_worker *worker);
struct kcoro_worker *kcoro_worker_create(const char *name, ...);
void kcoro_worker_remove(struct kcoro_worker *worker);

#endif  /* _KCORO_H_ */
