/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BYTEFILED_H_
#define _BYTEFILED_H_

#include <proc.h>

#define BYTEFILL_FIXED_ARGS void *buffer, size_t size
#define BYTEFILL_DECLARE_ARGS(FUNCSTATIC, FUNCNAME, FILLTYPE, FILLFUNC, FILLARGS, ...)  \
FUNCSTATIC void FUNCNAME(__VA_ARGS__)                                                   \
{                                                                                       \
    uint8_t *fillptr = buffer;                                                          \
    FILLTYPE value;                                                                     \
                                                                                        \
    while (size >= sizeof(FILLTYPE)) {                                                  \
        unaligned_set((FILLTYPE *)fillptr, FILLFUNC FILLARGS);                          \
        fillptr += sizeof(FILLTYPE);                                                    \
        size -= sizeof(FILLTYPE);                                                       \
    }                                                                                   \
                                                                                        \
    if (size) {                                                                         \
        value = FILLFUNC FILLARGS;                                                      \
        do {                                                                            \
            *fillptr++ = (uint8_t)value;                                                \
            value >>= BITS_PER_BYTE;                                                    \
        } while (--size);                                                               \
    }                                                                                   \
}

#define BYTEFILL_DECLARE_PREFIX(FUNCSTATIC, FUNCNAME, FILLTYPE, FILLFUNC, FILLARGS, ...) \
    BYTEFILL_DECLARE_ARGS(FUNCSTATIC, FUNCNAME, FILLTYPE, FILLFUNC, FILLARGS, __VA_ARGS__, BYTEFILL_FIXED_ARGS)

#define BYTEFILL_DECLARE_SUFFIX(FUNCSTATIC, FUNCNAME, FILLTYPE, FILLFUNC, FILLARGS, ...) \
    BYTEFILL_DECLARE_ARGS(FUNCSTATIC, FUNCNAME, FILLTYPE, FILLFUNC, FILLARGS, BYTEFILL_FIXED_ARGS, __VA_ARGS__)

extern void bytefill_short(void *buffer, size_t size, short (*fillfn)(void *pdata), void *pdata);
extern void bytefill_int(void *buffer, size_t size, int (*fillfn)(void *pdata), void *pdata);
extern void bytefill_long(void *buffer, size_t size, long (*fillfn)(void *pdata), void *pdata);
extern void bytefill_longlong(void *buffer, size_t size, long long (*fillfn)(void *pdata), void *pdata);

extern void bytefill_u16(void *buffer, size_t size, uint16_t (*fillfn)(void *pdata), void *pdata);
extern void bytefill_u32(void *buffer, size_t size, uint32_t (*fillfn)(void *pdata), void *pdata);
extern void bytefill_u64(void *buffer, size_t size, uint64_t (*fillfn)(void *pdata), void *pdata);

#endif  /* _BYTEFILED_H_ */
