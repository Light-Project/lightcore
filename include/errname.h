/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ERRNAME_H_
#define _ERRNAME_H_

#include <types.h>
#include <state.h>

struct errname_entry {
    state errnum;
    const char *name;
    const char *info;
};

extern const struct errname_entry errname_table[];
extern struct errname_entry *errname_find(state error);
extern const char *errname(state error, const char **infop);

#endif /* _ERRNAME_H_ */
