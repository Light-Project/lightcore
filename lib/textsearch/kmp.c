/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <ctype.h>
#include <kmalloc.h>
#include <initcall.h>
#include <textsearch.h>

struct kmp_context {
    struct ts_context tsc;
    char *pattern;
    unsigned int pattern_len;
    unsigned int prefix_table[0];
};

#define ts_to_kmp(ptr) \
    container_of(ptr, struct kmp_context, tsc)

static const char *kmp_pattern_get(struct ts_context *tsc)
{
    struct kmp_context *kctx = ts_to_kmp(tsc);
    return kctx->pattern;
}

static unsigned int kmp_pattern_len(struct ts_context *tsc)
{
    struct kmp_context *kctx = ts_to_kmp(tsc);
    return kctx->pattern_len;
}

static unsigned int kmp_find(struct ts_context *tsc, struct ts_state *tss)
{
    #define find_pattern() (icase ? toupper(text[index]) : text[index])
    bool icase = ts_test_ignorecase(tsc);
    struct kmp_context *kctx = ts_to_kmp(tsc);
    unsigned int consumed = tss->offset;
    unsigned int length, index, match = 0;
    const char *text;

    for (;;) {
        length = tsc->next_block(tsc, tss, consumed, (const void **)&text);
        if (unlikely(!length))
            return UINT_MAX;

        for (index = 0; index < length; ++index) {
            while (match && kctx->pattern[match] != find_pattern())
                match = kctx->prefix_table[match - 1];

            if (kctx->pattern[match] == find_pattern())
                match++;

            if (unlikely(match == kctx->pattern_len)) {
                tss->offset = consumed + index + 1;
                return tss->offset - kctx->pattern_len;
            }
        }

        consumed += length;
    }
}

static inline void compute_prefix(struct kmp_context *kctx)
{
    unsigned int index, match;

    for (match = 0, index = 1; index < kctx->pattern_len; ++index) {
        while (match && kctx->pattern[index] != kctx->pattern[match])
            match = kctx->prefix_table[match - 1];

        if (kctx->pattern[index] == kctx->pattern[match])
            match++;

        kctx->prefix_table[index] = match;
    }
}

static struct ts_context *kmp_prepare(const void *pattern, size_t len,
                                      gfp_t gfp, unsigned long flags)
{
    unsigned int prefix_size = sizeof(unsigned int) * len;
    struct kmp_context *kctx;
    unsigned int index;

    kctx = kmalloc(sizeof(*kctx) + prefix_size + len, gfp);
    if (unlikely(!kctx))
        return ERR_PTR(-ENOMEM);

    kctx->tsc.flags = flags;
    kctx->pattern_len = len;
    kctx->pattern = (void *)kctx + sizeof(*kctx) + prefix_size;

    if (!ts_test_ignorecase(&kctx->tsc))
        memcpy(kctx->pattern, pattern, len);
    else for (index = 0; index < len; ++index)
        kctx->pattern[index] = toupper(((char *)pattern)[index]);
    compute_prefix(kctx);

    return &kctx->tsc;
}

static void kmp_destroy(struct ts_context *tsc)
{
    struct kmp_context *kctx = ts_to_kmp(tsc);
    kfree(kctx);
}

struct ts_algorithm kmp_algorithm = {
    .name = "kmp",
    .find = kmp_find,
    .prepare = kmp_prepare,
    .destroy = kmp_destroy,
    .pattern_get = kmp_pattern_get,
    .pattern_len = kmp_pattern_len,
};

static state kmp_init(void)
{
    return textsearch_register(&kmp_algorithm);
}
postcore_initcall(kmp_init);
