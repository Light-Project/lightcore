/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <stringhash.h>
#include <proc.h>
#include <asm/wordops.h>
#include <export.h>

uint64_t __weak stringhash(const char *str, unsigned long salt)
{
    const struct wordops constants = WORDOPS_CONSTANTS;
    unsigned long value, temp, hash = 0;
    intptr_t position;

    position = 0;
    value = unaligned_get_ulong(str);

    while (!word_has_zero(&constants, &temp, value)) {
        HASH_MIX(hash, salt, value);
        position += BYTES_PER_LONG;
        value = unaligned_get_ulong(str + position);
    }

    temp = word_prep_mask(&constants, temp, value);
    temp = word_create_mask(temp);
    hash ^= value & word_zero_mask(temp);

    return strhash_create(fold_hash(hash, salt), word_find_zero(temp));
}
EXPORT_SYMBOL(stringhash);

unsigned long __weak pjwhash(const char *str)
{
    unsigned long value, hash = 0;

    while (*str) {
        hash <<= 4;
        hash += *str++;

        value = hash & (0xfUL << (PJWHASH_BITS - 4));
        if (value) {
            hash ^= value >> (PJWHASH_BITS - 8);
            hash ^= value;
        }
    }

    return hash;
}
EXPORT_SYMBOL(pjwhash);
