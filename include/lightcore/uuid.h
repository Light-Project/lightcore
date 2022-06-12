/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_UUID_H_
#define _LIGHTCORE_UUID_H_

#include <lightcore/types.h>

#define GUID_SIZE 16

typedef struct {
    __u8 byte[GUID_SIZE];
} guid_t;

#endif  /* _LIGHTCORE_UUID_H_ */
