/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _GLOB_H_
#define _GLOB_H_

#include <types.h>
#include <stddef.h>

extern __pure bool glob_match(const char *glob, const char *string);

#endif  /* _GLOB_H_ */
