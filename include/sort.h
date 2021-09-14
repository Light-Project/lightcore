/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SORT_H_
#define _SORT_H_

#include <list.h>

typedef int (*list_cmp_t)(struct list_head *, struct list_head *, void *);
void sort_list(struct list_head *head, list_cmp_t cmp, void *data);

#endif  /* _SORT_H_ */
