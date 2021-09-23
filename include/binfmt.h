/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BINFMT_H_
#define _BINFMT_H_

#include <types.h>
#include <list.h>

struct binfmt_type {
    struct list_head list;
    state (*load_bin)();
    state (*load_lib)();
};

#endif  /* _BINFMT_H_ */
