/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <types.h>

struct rng_device;

extern state random_generator_change(struct rng_device *rdev);
extern long random_bytes(void *buff, size_t len);
extern long random_long(void);

#endif /* _RANDOM_H_ */
