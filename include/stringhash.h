/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STRINGHASH_H_
#define _STRINGHASH_H_

#include <hash.h>
#include <bitops.h>

#define strhash_create(hash, len) ((uint64_t)(len) << 32 | (uint32_t)(hash))
#define strhash_hash(hashlen) ((uint32_t)((hashlen) >> 0))
#define strhash_len(hashlen) ((uint32_t)((hashlen) >> 32))

#if BITS_PER_LONG == 32
# define HASH_MIX(x, y, a) (    \
    x ^= (a),                   \
    y ^= x, x = rol32(x, 7),    \
    x += y, y = rol32(y, 20),   \
    y *= 9                      \
)

static __always_inline uint32_t
fold_hash(unsigned long x, unsigned long y)
{
    return __hash32(y ^ __hash32(x));
}
#else /* BITS_PER_LONG == 64 */
# define HASH_MIX(x, y, a) (    \
    x ^= (a),                   \
    y ^= x, x = rol64(x, 12),   \
    x += y, y = rol64(y, 45),   \
    y *= 9                      \
)

static __always_inline uint32_t
fold_hash(unsigned long x, unsigned long y)
{
    return __hash64(y ^ __hash64(x)) >> 32;
}
#endif /* BITS_PER_LONG == 64 */

#define PJWHASH_BITS 32

extern uint64_t stringhash(const char *str, unsigned long salt);
extern unsigned long pjwhash(const char *str);

#endif  /* _STRINGHASH_H_ */
