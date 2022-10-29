/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <textsearch.h>
#include <export.h>

static unsigned int linear_next(struct ts_context *tsc, struct ts_state *tss,
                                unsigned int consumed, const void **dest)
{
    struct ts_linear *linear = tss->pdata;

	if (likely(consumed < linear->len)) {
		*dest = linear->data + consumed;
		return linear->len - consumed;
	}

    return 0;
}

unsigned int textsearch_linear_find(struct ts_context *tsc, struct ts_linear *linear,
                                    const void *data, unsigned int len)
{
    struct ts_algorithm *algo = tsc->algo;

    linear->data = data;
    linear->len = len;
    linear->tss.offset = 0;
    linear->tss.pdata = linear;
    tsc->next_block = linear_next;

    return algo->find(tsc, &linear->tss);
}
EXPORT_SYMBOL(textsearch_linear_find);

unsigned int textsearch_linear_next(struct ts_context *tsc, struct ts_linear *linear)
{
    struct ts_algorithm *algo = tsc->algo;
    return algo->find(tsc, &linear->tss);
}
EXPORT_SYMBOL(textsearch_linear_next);
