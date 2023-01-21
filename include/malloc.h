/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <types.h>
#include <stddef.h>

extern __malloc void *malloc(size_t size);
extern __malloc void *zalloc(size_t size);
extern __malloc void *calloc(size_t num, size_t size);
extern __malloc void *realloc(void *block, size_t rsize);
extern void free(void *block);

#endif  /* _MALLOC_H_ */
