/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CHARDEV_H_
#define _CHARDEV_H_

#include <filesystem.h>

struct chardev {
    struct list_head list;
};

#endif  /* _CHARDEV_H_ */
