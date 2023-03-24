/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SORT_H_
#define _SORT_H_

#include <types.h>
#include <state.h>
#include <stddef.h>

typedef long (*sort_cmp_t)(const void *nodea, const void *nodeb, void *pdata);
extern state qsort(void *base, size_t number, size_t cells, sort_cmp_t cmp, void *pdata);
extern state bsort(void *base, size_t number, size_t cells, sort_cmp_t cmp, void *pdata);

#endif /* _SORT_H_ */
