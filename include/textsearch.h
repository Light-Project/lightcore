/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TEXTSEARCH_H_
#define _TEXTSEARCH_H_

#include <list.h>
#include <error.h>
#include <bitflags.h>
#include <mm/gfp.h>

enum ts_flags {
    __TS_IGNORECASE = 0,
};

#define TS_IGNORECASE BIT(__TS_IGNORECASE)

/**
 * struct ts_state - search state.
 * @offset: offset for next match.
 * @offset: search context private data.
 */
struct ts_state {
    unsigned int offset;
    void *pdata;
};

struct ts_linear {
    struct ts_state tss;
    const char *data;
    unsigned int len;
};

/**
 * struct ts_context - search context.
 * @ops: chosen search algorithm.
 * @flags: user flags.
 * @next_block: fetch next block of data.
 */
struct ts_context {
    struct ts_algorithm *algo;
    unsigned long flags;
    unsigned int (*next_block)(struct ts_context *tsc, struct ts_state *tss,
                               unsigned int consumed, const void **dest);
};

/**
 * struct ts_algorithm - search algorithm operations.
 * @name: name of search algorithm.
 * @prepare: initialization function to prepare a search.
 * @destroy: destroy algorithm specific parts of a search configuration.
 * @find: find the next occurrence of the pattern.
 * @pattern_get: return pointer of pattern.
 * @pattern_len: return length of pattern.
 */
struct ts_algorithm {
    struct list_head list;
    const char *name;

    struct ts_context *(*prepare)(const void *pattern, size_t len, gfp_t gfp, unsigned long flags);
    void (*destroy)(struct ts_context *tsc);
    unsigned int (*find)(struct ts_context *tsc, struct ts_state *tss);
    const void *(*pattern_get)(struct ts_context *tsc);
    unsigned int (*pattern_len)(struct ts_context *tsc);
};

GENERIC_STRUCT_BITOPS(ts, struct ts_context, flags, false)
GENERIC_STRUCT_FLAG(ts, struct ts_context, flags, ignorecase, __TS_IGNORECASE)

static inline unsigned int textsearch_find(struct ts_context *tsc, struct ts_state *tss)
{
    struct ts_algorithm *algo = tsc->algo;
    tss->offset = 0;
    return algo->find(tsc, tss);
}

static inline unsigned int textsearch_next(struct ts_context *tsc, struct ts_state *tss)
{
    struct ts_algorithm *algo = tsc->algo;
    return algo->find(tsc, tss);
}

static inline const void *textsearch_pattern_get(struct ts_context *tsc)
{
    struct ts_algorithm *algo = tsc->algo;
    return algo->pattern_get(tsc);
}

static inline unsigned int textsearch_pattern_len(struct ts_context *tsc)
{
    struct ts_algorithm *algo = tsc->algo;
    return algo->pattern_len(tsc);
}

static inline void textsearch_destroy(struct ts_context *tsc)
{
    struct ts_algorithm *algo = tsc->algo;
    algo->destroy(tsc);
}

extern unsigned int textsearch_linear_find(struct ts_context *tsc, struct ts_linear *linear, const void *data, unsigned int len);
extern unsigned int textsearch_linear_next(struct ts_context *tsc, struct ts_linear *linear);

extern struct ts_context *textsearch_prepare(const char *name, const void *pattern, size_t len, gfp_t gfp, unsigned long flags);
extern state textsearch_register(struct ts_algorithm *algo);
extern state textsearch_unregister(struct ts_algorithm *algo);

#endif  /* _TEXTSEARCH_H_ */
