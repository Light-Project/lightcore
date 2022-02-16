/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _HASH_H_
#define _HASH_H_

#include <types.h>

#define GOLDEN_RATIO_32 0x61C88647U
#define GOLDEN_RATIO_64 0x61C8864680B583EBULL

#define hashlen_create(hash, len)   ((uint64_t)(len << 32) | (uint32_t)(hash))
#define hashlen_hash(hashlen)       ((uint32_t)((hashlen) >> 0))
#define hashlen_len(hashlen)        ((uint32_t)((hashlen) >> 32))

#endif  /* _HASH_H_ */
