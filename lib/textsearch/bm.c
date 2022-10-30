/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <ctype.h>
#include <kmalloc.h>
#include <initcall.h>
#include <textsearch.h>

struct bm_context {
    struct ts_context tsc;
    uint8_t *pattern;
    unsigned int pattern_len;
    unsigned int bad_shift[UINT8_MAX];
    unsigned int good_shift[0];
};

#define ts_to_bm(ptr) \
    container_of(ptr, struct bm_context, tsc)

static const void *bm_pattern_get(struct ts_context *tsc)
{
    struct bm_context *bctx = ts_to_bm(tsc);
    return bctx->pattern;
}

static unsigned int bm_pattern_len(struct ts_context *tsc)
{
    struct bm_context *bctx = ts_to_bm(tsc);
    return bctx->pattern_len;
}

static unsigned int bm_find(struct ts_context *tsc, struct ts_state *tss)
{
    bool icase = ts_test_ignorecase(tsc);
    struct bm_context *bctx = ts_to_bm(tsc);
    unsigned int bad_shift, good_shift, consumed = tss->offset;
    unsigned int length, index, shift = bctx->pattern_len - 1;
    const uint8_t *text;

    for (;;) {
        length = tsc->next_block(tsc, tss, consumed, (const void **)&text);
        if (unlikely(!length))
            return UINT_MAX;

        while (shift < length) {
            for (index = 0; index < bctx->pattern_len; ++index) {
                if (bctx->pattern[bctx->pattern_len - index - 1] !=
                    (icase ? toupper(text[shift - index]) : text[shift - index]))
                    break;
            }

            if (unlikely(index == bctx->pattern_len))
                return consumed + shift - bctx->pattern_len + 1;

            bad_shift = shift - index + bctx->bad_shift[text[shift - index]];
            good_shift = shift + bctx->good_shift[index];
            shift = max(bad_shift, good_shift);
        }

        consumed += length;
    }
}

static bool subpattern(const uint8_t *pattern, int i, int j, int g)
{
    int x = i + g - 1, y = j + g - 1;

    while (y && pattern[x--] == pattern[y--]) {
        if (y < 0)
            return true;
        if (!--g)
            return pattern[i-1] != pattern[j-1];
    }

    return false;
}

static inline void compute_prefix(struct bm_context *bctx)
{
    int index, start, count;

    for (index = 0; index < UINT8_MAX; ++index)
        bctx->bad_shift[index] = bctx->pattern_len;

    for (index = 0; index < bctx->pattern_len - 1; ++index)
        bctx->bad_shift[bctx->pattern[index]] = bctx->pattern_len - index - 1;

    bctx->good_shift[0] = 1;
    for (index = 1; index < bctx->pattern_len; ++index)
        bctx->good_shift[index] = bctx->pattern_len;

    for (index = bctx->pattern_len - 1, start = 1; index; ++start, --index) {
        for (count = index - 1; count >= 1 - start; --count) {
            if (subpattern(bctx->pattern, index, start, count)) {
                bctx->good_shift[count] = bctx->pattern_len - start - count;
                break;
            }
        }
    }
}

static struct ts_context *bm_prepare(const void *pattern, size_t len,
                                     gfp_t gfp, unsigned long flags)
{
    unsigned int good_size = sizeof(unsigned int) * len;
    struct bm_context *bctx;
    unsigned int index;

    bctx = kmalloc(sizeof(*bctx) + good_size + len, gfp);
    if (unlikely(!bctx))
        return ERR_PTR(-ENOMEM);

    bctx->tsc.flags = flags;
    bctx->pattern_len = len;
    bctx->pattern = (void *)bctx + sizeof(*bctx) + good_size;

    if (!ts_test_ignorecase(&bctx->tsc))
        memcpy(bctx->pattern, pattern, len);
    else for (index = 0; index < len; ++index)
        bctx->pattern[index] = toupper(((char *)pattern)[index]);
    compute_prefix(bctx);

    return &bctx->tsc;
}

static void bm_destroy(struct ts_context *tsc)
{
    struct bm_context *bctx = ts_to_bm(tsc);
    kfree(bctx);
}

struct ts_algorithm bm_algorithm = {
    .name = "bm",
    .find = bm_find,
    .prepare = bm_prepare,
    .destroy = bm_destroy,
    .pattern_get = bm_pattern_get,
    .pattern_len = bm_pattern_len,
};

static state bm_init(void)
{
    return textsearch_register(&bm_algorithm);
}
postcore_initcall(bm_init);
