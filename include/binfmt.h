/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BINFMT_H_
#define _BINFMT_H_

#include <types.h>
#include <list.h>
#include <filesystem.h>

struct binfmt_type {
    struct list_head list;
    const char *name;
    state (*load_bin)(struct file *);
    state (*load_lib)(struct file *);
};

state binfmt_register(struct binfmt_type *);
void binfmt_unregister(struct binfmt_type *);

#endif /* _BINFMT_H_ */
