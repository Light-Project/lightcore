/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <ctype.h>
#include <kmalloc.h>
#include <initcall.h>
#include <textsearch.h>

struct sunday_context {
    struct ts_context tsc;
    unsigned int pattern_len;
    unsigned int shift_table[UINT8_MAX];
    uint8_t pattern[0];
};

#define ts_to_sunday(ptr) \
    container_of(ptr, struct sunday_context, tsc)

static const void *sunday_pattern_get(struct ts_context *tsc)
{
    struct sunday_context *sctx = ts_to_sunday(tsc);
    return sctx->pattern;
}

static unsigned int sunday_pattern_len(struct ts_context *tsc)
{
    struct sunday_context *sctx = ts_to_sunday(tsc);
    return sctx->pattern_len;
}

static unsigned int sunday_find(struct ts_context *tsc, struct ts_state *tss)
{
    bool icase = ts_test_ignorecase(tsc);
    struct sunday_context *sctx = ts_to_sunday(tsc);
    unsigned int consumed = tss->offset;
    unsigned int length, index, shift = 0;
    const uint8_t *text;

    for (;;) {
        length = tsc->next_block(tsc, tss, consumed, (const void **)&text);
        if (unlikely(!length))
            return UINT_MAX;

        while (sctx->pattern_len + shift <= length) {
            for (index = 0; index < sctx->pattern_len; ++index) {
                if (sctx->pattern[index] !=
                    (icase ? toupper(text[shift + index]) : text[shift + index]))
                    break;
            }

            if (unlikely(index == sctx->pattern_len))
                return consumed + shift;

            if (sctx->pattern_len >= length)
                break;

            shift += sctx->shift_table[text[shift + sctx->pattern_len]];
        }

        consumed += length;
    }
}

static inline void compute_prefix(struct sunday_context *sctx)
{
    unsigned int index;

    for (index = 0; index < UINT8_MAX; ++index)
        sctx->shift_table[index] = sctx->pattern_len + 1;

    for (index = 0; index < sctx->pattern_len; ++index)
        sctx->shift_table[sctx->pattern[index]] = sctx->pattern_len - index;
}

static struct ts_context *sunday_prepare(const void *pattern, size_t len,
                                         gfp_t gfp, unsigned long flags)
{
    struct sunday_context *sctx;
    unsigned int index;

    sctx = kmalloc(sizeof(*sctx) + len, gfp);
    if (unlikely(!sctx))
        return ERR_PTR(-ENOMEM);

    sctx->tsc.flags = flags;
    sctx->pattern_len = len;

    if (!ts_test_ignorecase(&sctx->tsc))
        memcpy(sctx->pattern, pattern, len);
    else for (index = 0; index < len; ++index)
        sctx->pattern[index] = toupper(((char *)pattern)[index]);
    compute_prefix(sctx);

    return &sctx->tsc;
}

static void sunday_destroy(struct ts_context *tsc)
{
    struct sunday_context *sctx = ts_to_sunday(tsc);
    kfree(sctx);
}

struct ts_algorithm sunday_algorithm = {
    .name = "sunday",
    .find = sunday_find,
    .prepare = sunday_prepare,
    .destroy = sunday_destroy,
    .pattern_get = sunday_pattern_get,
    .pattern_len = sunday_pattern_len,
};

static state sunday_init(void)
{
    return textsearch_register(&sunday_algorithm);
}
postcore_initcall(sunday_init);
