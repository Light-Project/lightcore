/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PEVENT_H_
#define _PEVENT_H_

#include <state.h>
#include <list.h>

struct pevent {
    struct list_head list;
    state (*suspend)(void);
    void (*resume)(void);
    void (*shutdown)(void);
};

extern state pevent_suspend(void);
extern void pevent_resume(void);
extern void pevent_shutdown(void);

extern state pevent_register(struct pevent *);
extern void pevent_unregister(struct pevent *);

#endif  /* _PEVENT_H_ */
