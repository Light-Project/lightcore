/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ACTION_H_
#define _ACTION_H_

#include <types.h>
#include <stddef.h>
#include <state.h>

typedef state (*action_func_t)(void *data);

struct action {
    action_func_t func;
    void *data;
};

#define ACTION_STATIC(Func, Data) { \
    .func = (Func), .data = (Data), \
}

#define ACTION_INIT(Func, Data) \
    (struct action) ACTION_STATIC(Func, Data)

#define DEFINE_ACTION(Name, Func, Data) \
    struct action Name = ACTION_INIT(Func, Data)

static inline void action_init(struct action *act, action_func_t func, void *data)
{
    *act = ACTION_INIT(func, data);
}

static inline void action_update(struct action *act, void *data)
{
    act->data = data;
}

static inline void action_clear(struct action *act)
{
    act->func = NULL;
}

static inline state action_call(struct action *act)
{
    if (likely(act->func))
        return act->func(act->data);
    return -ENODATA;
}

#endif  /* _ACTION_H_ */
